#include "sendmodel.h"
#include <QDataStream>
#include <QHostAddress>
#include <math.h>

// 定义文件块大小为64KB
const qint64 BLOCK_SIZE = 64 * 1024;

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
    if (!m_transferActive || m_currentFileIndex < 0 || m_currentFileIndex >= m_files.size()) {
        return;
    }
    
    // 检查socket的写缓冲区是否为空，如果为空则表示所有数据都已发送
    if (m_socket->bytesToWrite() == 0) {
        // 当前文件已完全发送，进入下一个文件
        sendNextFile();
    }
}

void SendModel::setParentPath(const QString &path){
    parentPath=path;
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
    m_socket->write(header);
    
    // 直接发送整个文件内容
    qint64 fileSize = m_files[m_currentFileIndex].fileSize;
    QByteArray fileContent = m_currentFile->readAll();
    
    if (fileContent.size() != fileSize) {
        emit transferError("读取文件内容错误: " + m_files[m_currentFileIndex].fileName);
        updateFileStatus(m_currentFileIndex, FileStatus::Failed);
        m_currentFile->close();
        sendNextFile();
        return;
    }
    
    // 发送整个文件内容
    qint64 bytesSent = m_socket->write(fileContent);
    
    if (bytesSent <= 0) {
        emit transferError("发送文件内容错误: " + m_files[m_currentFileIndex].fileName);
        updateFileStatus(m_currentFileIndex, FileStatus::Failed);
        m_currentFile->close();
        sendNextFile();
        return;
    }
    
    m_bytesSent = fileSize;
    m_files[m_currentFileIndex].bytesSent = m_bytesSent;
    
    // 更新文件状态
    m_currentFile->close();
    updateFileStatus(m_currentFileIndex, FileStatus::Completed);
    
    // 发送文件完成信号
    emit fileCompleted(m_currentFileIndex);
    emit transferProgress(m_currentFileIndex, fileSize, fileSize);
    
    // 等待数据全部写入缓冲区
    // 不会立即调用sendNextFile，而是等待onBytesWritten信号
}

void SendModel::updateFileStatus(int index, FileStatus status)
{
    if (index >= 0 && index < m_files.size()) {
        m_files[index].status = status;
    }
}
