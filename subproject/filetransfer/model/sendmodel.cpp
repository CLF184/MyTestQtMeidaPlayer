#include "sendmodel.h"
#include <QDataStream>
#include <QHostAddress>
#include <math.h>

// 定义文件块大小为1024kb
const qint64 BLOCK_SIZE = 1024 * 1024;

SendModel::SendModel(QObject *parent)
    : QObject(parent),
      m_targetPort(8000),
      m_socket(nullptr),
      m_currentFile(nullptr),
      m_currentFileIndex(-1),
      m_transferActive(false),
      m_bytesSent(0)
{
    // 初始化Socket
    m_socket = new QTcpSocket(this);
    connect(m_socket, &QTcpSocket::connected, this, &SendModel::onSocketConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &SendModel::onSocketDisconnected);
    connect(m_socket, &QTcpSocket::bytesWritten, this, &SendModel::onBytesWritten);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &SendModel::onSocketError);
}

SendModel::~SendModel()
{
    if (m_currentFile && m_currentFile->isOpen()) {
        m_currentFile->close();
        delete m_currentFile;
    }

    if (m_socket && m_socket->isOpen()) {
        m_socket->disconnect();
        m_socket->close();
    }
}

bool SendModel::addFile(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isFile() || !fileInfo.isReadable()) {
        return false;
    }

    // 检查文件是否已在列表中
    for (const FileItem &item : m_files) {
        if (item.filePath == filePath) {
            return false; // 文件已存在
        }
    }

    FileItem newItem;
    newItem.filePath = filePath;
    newItem.fileName = fileInfo.fileName();
    newItem.fileSize = fileInfo.size();
    newItem.status = FileStatus::Ready;
    newItem.bytesSent = 0;

    m_files.append(newItem);
    parentPath.clear();
    return true;
}

bool SendModel::removeFile(int index)
{
    if (index < 0 || index >= m_files.size() || m_transferActive) {
        return false;
    }

    m_files.removeAt(index);
    return true;
}

void SendModel::clearFiles()
{
    if (!m_transferActive) {
        m_files.clear();
        parentPath.clear();
    }
}

QList<FileItem> SendModel::getFiles() const
{
    return m_files;
}

int SendModel::getFileCount() const
{
    return m_files.size();
}

void SendModel::setTargetAddress(const QString &address)
{
    m_targetAddress = address;
}

void SendModel::setTargetPort(int port)
{
    m_targetPort = port;
}

bool SendModel::startTransfer()
{
    if (m_transferActive || m_files.isEmpty()) {
        return false;
    }

    // 重置状态
    m_currentFileIndex = -1;
    m_bytesSent = 0;

    // 连接到目标服务器
    m_socket->abort();
    m_socket->connectToHost(QHostAddress(m_targetAddress), m_targetPort);

    m_transferActive = true;
    return true;
}

void SendModel::cancelTransfer()
{
    if (!m_transferActive) {
        return;
    }

    if (m_currentFile && m_currentFile->isOpen()) {
        m_currentFile->close();
        delete m_currentFile;
        m_currentFile = nullptr;
    }

    // 更新当前文件状态
    if (m_currentFileIndex >= 0 && m_currentFileIndex < m_files.size()) {
        updateFileStatus(m_currentFileIndex, FileStatus::Cancelled);
    }

    m_socket->abort();
    m_transferActive = false;
    emit transferCancelled();
}

void SendModel::onSocketConnected()
{
    // 开始发送第一个文件
    m_currentFileIndex = -1;
    emit transferStarted();
    sendNextFile();
}

void SendModel::onSocketDisconnected()
{
    if (m_transferActive) {
        // 如果还有文件未发送，则认为传输被中断
        if (m_currentFileIndex < m_files.size() - 1) {
            emit transferError("连接断开，传输中断");
        }

        m_transferActive = false;
    }
}

void SendModel::onBytesWritten(qint64 bytes)
{
    if (!m_transferActive || m_currentFileIndex < 0) {
        return;
    }

    // 首先处理文件头部分的字节
    if (m_headerBytesSent < m_headerSize) {
        qint64 headerBytesWritten = qMin(bytes, m_headerSize - m_headerBytesSent);
        m_headerBytesSent += headerBytesWritten;

        if (m_headerBytesSent >= m_headerSize) {
            // 文件头已完全发送，计算可能属于文件内容的字节数
            qint64 overflowBytes = bytes - headerBytesWritten;
            if (overflowBytes > 0) {
                // 有一部分字节是文件内容
                m_bytesSent += overflowBytes;
                m_files[m_currentFileIndex].bytesSent = m_bytesSent;
                emit transferProgress(m_currentFileIndex, m_bytesSent, m_files[m_currentFileIndex].fileSize);
            }
            // 开始发送文件内容
            sendFileData();
        }
        return;
    }

    // 更新文件内容已发送字节数
    m_bytesSent += bytes;

    // 确保不超过文件大小
    if (m_bytesSent > m_files[m_currentFileIndex].fileSize) {
        qWarning() << "警告：发送的字节数超过文件大小："
                  << m_bytesSent << "/" << m_files[m_currentFileIndex].fileSize
                  << "文件：" << m_files[m_currentFileIndex].fileName;
        m_bytesSent = m_files[m_currentFileIndex].fileSize;
    }

    m_files[m_currentFileIndex].bytesSent = m_bytesSent;

    // 减少发送进度的频率，避免过于频繁的信号发射
    static int sentCount = 0;
    sentCount++;
    if(sentCount % 10 == 0|| m_bytesSent >= m_files[m_currentFileIndex].fileSize) {
        sentCount = 0; // 重置计数器
        // 发送进度信号
        emit transferProgress(m_currentFileIndex, m_bytesSent, m_files[m_currentFileIndex].fileSize);
    }
    
    // 继续发送文件数据
    sendFileData();
}

void SendModel::onSocketError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    emit transferError(m_socket->errorString());
    cancelTransfer();
}

void SendModel::sendNextFile()
{
    // 检查是否所有文件都已发送
    if (++m_currentFileIndex >= m_files.size()) {
        m_transferActive = false;
        emit transferCompleted();
        return;
    }

    // 准备发送当前文件
    m_bytesSent = 0;
    updateFileStatus(m_currentFileIndex, FileStatus::Sending);

    // 打开文件
    if (m_currentFile) {
        delete m_currentFile;
    }
    m_currentFile = new QFile(m_files[m_currentFileIndex].filePath);
    if (!m_currentFile->open(QIODevice::ReadOnly)) {
        emit transferError("无法打开文件: " + m_files[m_currentFileIndex].fileName);
        updateFileStatus(m_currentFileIndex, FileStatus::Failed);
        sendNextFile(); // 跳到下一个文件
        return;
    }

    // 发送文件头信息
    QByteArray header;
    QDataStream headerStream(&header, QIODevice::WriteOnly);
    headerStream.setVersion(QDataStream::Qt_6_0);

    // 文件头格式: 文件名长度(qint32) + 文件名内容 + 文件大小(qint64)
    QByteArray fileNameData;
    qDebug()<<"父文件夹路径"<<parentPath;
    if(parentPath.length()==0){
        fileNameData = m_files[m_currentFileIndex].fileName.toUtf8();
    }
    else{
        fileNameData = m_files[m_currentFileIndex].filePath.remove(parentPath).toUtf8();
    }
    qDebug()<<"文件路径"<<fileNameData; 
    headerStream << (qint32)fileNameData.size();
    // 写入文件名
    headerStream.writeRawData(fileNameData.constData(), fileNameData.size());
    // 写入文件大小
    headerStream << (qint64)m_files[m_currentFileIndex].fileSize;

    // 记录文件头长度，会在onBytesWritten中用到
    m_headerSize = header.size();
    m_headerBytesSent = 0;

    m_socket->write(header);

    // 文件数据会在onBytesWritten中被发送
}

void SendModel::sendFileData()
{
    if (!m_currentFile || !m_currentFile->isOpen()) {
        return;
    }

    // 计算还需发送的字节数
    qint64 bytesRemaining = m_files[m_currentFileIndex].fileSize - m_bytesSent;
    if (bytesRemaining <= 0) {
        // 文件发送完成
        m_currentFile->close();
        updateFileStatus(m_currentFileIndex, FileStatus::Completed);
        emit fileCompleted(m_currentFileIndex);

        // 发送下一个文件
        sendNextFile();
        return;
    }

    // 读取文件块并发送，确保不超过剩余字节数
    qint64 bytesToRead = qMin(BLOCK_SIZE, bytesRemaining);

    // 检查文件是否可以读取请求的字节数
    if (m_currentFile->atEnd() || m_currentFile->pos() + bytesToRead > m_files[m_currentFileIndex].fileSize) {
        bytesToRead = m_files[m_currentFileIndex].fileSize - m_currentFile->pos();
        if (bytesToRead <= 0) {
            // 已经到达文件末尾，完成此文件传输
            m_currentFile->close();
            updateFileStatus(m_currentFileIndex, FileStatus::Completed);
            emit fileCompleted(m_currentFileIndex);
            sendNextFile();
            return;
        }
    }

    QByteArray block = m_currentFile->read(bytesToRead);

    if (block.isEmpty() && !m_currentFile->atEnd()) {
        // 读取失败但未到文件末尾
        emit transferError("读取文件时出错: " + m_files[m_currentFileIndex].fileName);
        updateFileStatus(m_currentFileIndex, FileStatus::Failed);
        m_currentFile->close();
        sendNextFile(); // 跳到下一个文件
        return;
    }

    // 可能会读取到比请求更少的字节数，这是正常的
    m_socket->write(block);
}

void SendModel::updateFileStatus(int index, FileStatus status)
{
    if (index >= 0 && index < m_files.size()) {
        m_files[index].status = status;
    }
}

void SendModel::setParentPath(const QString &path)
{
    parentPath = path;
}
