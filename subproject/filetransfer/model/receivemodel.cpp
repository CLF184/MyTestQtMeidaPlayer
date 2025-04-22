#include "receivemodel.h"
#include <QDataStream>
#include <QHostAddress>
#include <QDir>
#include <QRegularExpression>

ReceiveModel::ReceiveModel(QObject *parent)
    : QObject(parent),
      m_savePath(QDir::homePath())
{
    // 创建 QTcpServer
    m_server = new QTcpServer(this);
    
    // 连接信号槽
    connect(m_server, &QTcpServer::newConnection, this, &ReceiveModel::onNewConnection);
}

ReceiveModel::~ReceiveModel()
{
    stopServer();
}

// 实现启动服务器方法
bool ReceiveModel::startServer(const QString &ip, int port, const QString &savePath)
{
    // 设置保存路径
    m_savePath = savePath;
    
    // 如果服务器已经在监听，先停止它
    if (m_server->isListening()) {
        m_server->close();
    }
    
    // 启动服务器
    QHostAddress address;
    if (ip.isEmpty() || ip == "0.0.0.0" || ip == "any") {
        address = QHostAddress::Any;
    } else {
        address.setAddress(ip);
    }
    
    if (!m_server->listen(address, port)) {
        emit errorOccurred(tr("无法启动服务器: %1").arg(m_server->errorString()));
        emit serverError(tr("无法启动服务器: %1").arg(m_server->errorString()));
        return false;
    }
    
    emit serverStarted();
    return true;
}

void ReceiveModel::stopServer()
{
    if (m_server->isListening()) {
        // 关闭所有客户端连接
        for (QTcpSocket* socket : m_currentFiles.keys()) {
            socket->disconnect();
            socket->close();
            socket->deleteLater();
        }
        
        // 清理资源
        for (QFile* file : m_currentFiles.values()) {
            if (file && file->isOpen()) {
                file->close();
                delete file;
            }
        }
        
        m_currentFiles.clear();
        m_fileSizes.clear();
        m_bytesReceived.clear();
        m_fileNames.clear();
        m_fileIndices.clear();
        m_waitingForHeader.clear();
        
        // 停止服务器
        m_server->close();
        emit serverStopped();
    }
}

bool ReceiveModel::isListening() const
{
    return m_server->isListening();
}

void ReceiveModel::setSavePath(const QString &path)
{
    m_savePath = path;
    
    // 确保保存目录存在
    QDir dir(m_savePath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
}

QString ReceiveModel::getSavePath() const
{
    return m_savePath;
}

QList<ReceivedFileItem> ReceiveModel::getReceivedFiles() const
{
    return m_files;
}

void ReceiveModel::clearReceivedFiles()
{
    // 只清除已完成、取消或失败的文件
    QList<ReceivedFileItem> activeFiles;
    for (const ReceivedFileItem &item : m_files) {
        if (item.status == ReceiveStatus::Receiving) {
            activeFiles.append(item);
        }
    }
    m_files = activeFiles;
}

void ReceiveModel::onNewConnection()
{
    QTcpSocket *socket = m_server->nextPendingConnection();
    
    if (!socket) {
        return;
    }
    
    connect(socket, &QTcpSocket::readyRead, this, &ReceiveModel::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &ReceiveModel::onSocketDisconnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &ReceiveModel::onSocketError);
    
    // 初始化新连接的状态
    m_waitingForHeader[socket] = true;
    m_currentFiles[socket] = nullptr;
    m_bytesReceived[socket] = 0;
    
    // 发送新连接信号
    emit newConnection(socket->peerAddress().toString());
    emit clientConnected(socket->peerAddress().toString());
}

void ReceiveModel::onReadyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) {
        return;
    }
    
    // 处理文件头信息或文件数据
    if (m_waitingForHeader[socket]) {
        // 首先接收文件信息的头部
        if (socket->bytesAvailable() < (int)(sizeof(qint32) + sizeof(qint64))) {
            return; // 等待更多数据
        }
        
        // 读取文件名长度和文件大小
        QDataStream in(socket);
        in.setVersion(QDataStream::Qt_6_0);
        
        qint32 fileNameLength;
        qint64 fileSize;
        // 修正读取顺序，先读取文件名长度
        in >> fileNameLength;
        
        // 验证文件名长度的合理性
        if (fileNameLength <= 0 || fileNameLength > 1024) { // 限制最大文件名长度
            emit errorOccurred(tr("接收到无效的文件名长度: %1").arg(fileNameLength));
            socket->disconnectFromHost();
            return;
        }
        
        if (socket->bytesAvailable() < fileNameLength + sizeof(qint64)) {
            return; // 等待更多数据
        }
        
        // 读取文件名 - 使用QDataStream的readRawData而不是socket->read
        QByteArray fileNameData(fileNameLength, Qt::Uninitialized);
        in.readRawData(fileNameData.data(), fileNameLength);
        QString fileName = QString::fromUtf8(fileNameData);
        
        // 验证文件名
        if (fileName.isEmpty()) {
            emit errorOccurred(tr("接收到空文件名"));
            socket->disconnectFromHost();
            return;
        }
        
        // 读取文件大小
        in >> fileSize;
        
        // 验证文件大小
        if (fileSize <= 0 || fileSize > 4LL * 1024 * 1024 * 1024) { // 限制最大4GB
            emit errorOccurred(tr("接收到无效的文件大小: %1").arg(fileSize));
            socket->disconnectFromHost();
            return;
        }
        
        // 确保文件名安全（如果包含路径）
        fileName = sanitizeFileName(fileName);
        
        // 准备文件路径
        // 如果文件名包含路径分隔符，则需要创建对应的文件夹结构
        QString filePath = QDir(m_savePath).filePath(fileName);
        QFileInfo fileInfo(filePath);
        
        // 确保包含路径的文件能创建相应目录
        QDir dir = fileInfo.dir();
        if (!dir.exists()) {
            if (!dir.mkpath(".")) {
                emit errorOccurred(tr("无法创建目录: %1").arg(dir.path()));
                socket->disconnectFromHost();
                return;
            }
        }
        
        // 检查文件是否已存在，如果存在则重命名
        if (fileInfo.exists()) {
            int counter = 1;
            QString fileNameWithoutExt = fileInfo.completeBaseName();
            QString extension = fileInfo.suffix().isEmpty() ? "" : "." + fileInfo.suffix();
            QString dirPath = fileInfo.path();
            
            // 尝试添加数字后缀直到找到一个不存在的文件名
            do {
                filePath = QDir(dirPath).filePath(QString("%1_(%2)%3").arg(fileNameWithoutExt).arg(counter++).arg(extension));
                fileInfo.setFile(filePath);
            } while (fileInfo.exists() && counter < 1000);
            
            // 更新文件名以反映新路径（相对于保存路径）
            QString relativePath = QDir(m_savePath).relativeFilePath(filePath);
            fileName = relativePath;
        }
        
        QFile *file = new QFile(filePath);
        if (!file->open(QIODevice::WriteOnly)) {
            emit errorOccurred(tr("无法创建文件: %1").arg(filePath));
            emit serverError(tr("无法创建文件: %1").arg(filePath));
            delete file;
            socket->disconnectFromHost();
            return;
        }
        
        // 存储文件信息
        m_currentFiles[socket] = file;
        m_fileSizes[socket] = fileSize;
        m_bytesReceived[socket] = 0;
        m_fileNames[socket] = fileName;
        m_fileIndices[socket] = m_files.size();
        
        // 添加到接收文件列表
        ReceivedFileItem newItem;
        newItem.fileName = fileInfo.fileName(); // 只保存文件名部分
        newItem.fileSize = fileSize;
        newItem.filePath = filePath;
        newItem.savePath = m_savePath;
        newItem.bytesReceived = 0;
        newItem.completed = false;
        newItem.status = ReceiveStatus::Receiving;
        newItem.sender = socket->peerAddress().toString();
        m_files.append(newItem);
        
        emit fileReceiveStarted(newItem.fileName, fileSize);
        emit fileTransferStarted(newItem.fileName, fileSize);
        
        // 等待文件数据
        m_waitingForHeader[socket] = false;
    } else if (m_currentFiles.contains(socket) && m_currentFiles[socket]) {
        // 接收文件数据
        QFile *file = m_currentFiles[socket];
        qint64 fileSize = m_fileSizes[socket];
        qint64 bytesReceived = m_bytesReceived[socket];
        int fileIndex = m_fileIndices[socket];
        
        // 读取数据并写入文件
        qint64 bytesToRead = qMin(socket->bytesAvailable(), fileSize - bytesReceived);
        QByteArray buffer = socket->read(bytesToRead);
        file->write(buffer);
        
        // 更新接收进度
        bytesReceived += bytesToRead;
        m_bytesReceived[socket] = bytesReceived;
        
        // 更新接收文件列表
        if (fileIndex < m_files.size()) {
            m_files[fileIndex].bytesReceived = bytesReceived;
            if (bytesReceived >= fileSize) {
                m_files[fileIndex].completed = true;
                m_files[fileIndex].status = ReceiveStatus::Completed;
                m_files[fileIndex].completeTime = QDateTime::currentDateTime();
            }
        }
        
        // 发送进度更新信号
        emit fileReceiveProgress(fileIndex, bytesReceived, fileSize);
        emit fileTransferProgress(fileIndex, bytesReceived, fileSize);
        
        // 检查文件是否接收完成
        if (bytesReceived >= fileSize) {
            // 文件接收完成
            file->close();
            emit fileReceiveCompleted(fileIndex);
            emit fileReceived(m_fileNames[socket]);
            
            // 准备接收下一个文件
            m_currentFiles.remove(socket);
            m_fileSizes.remove(socket);
            m_bytesReceived.remove(socket);
            m_fileNames.remove(socket);
            m_fileIndices.remove(socket);
            m_waitingForHeader[socket] = true;
            
            delete file;
        }
    }
}

void ReceiveModel::onSocketDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) {
        return;
    }
    
    // 如果还有正在接收的文件，标记为失败
    if (m_currentFiles.contains(socket) && m_currentFiles[socket]) {
        QFile *file = m_currentFiles[socket];
        int fileIndex = m_fileIndices[socket];
        
        if (file->isOpen()) {
            file->close();
        }
        
        // 只有在尚未完成时才标记为失败
        if (m_bytesReceived[socket] < m_fileSizes[socket]) {
            updateFileStatus(fileIndex, ReceiveStatus::Failed);
            emit fileReceiveFailed(fileIndex, "连接断开，接收中断");
        }
        
        delete file;
    }
    
    // 清理资源
    m_currentFiles.remove(socket);
    m_fileSizes.remove(socket);
    m_bytesReceived.remove(socket);
    m_fileNames.remove(socket);
    m_fileIndices.remove(socket);
    m_waitingForHeader.remove(socket);
    
    // 删除socket
    socket->deleteLater();
    
    emit clientDisconnected();
}

void ReceiveModel::onSocketError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) {
        return;
    }
    
    int fileIndex = -1;
    if (m_fileIndices.contains(socket)) {
        fileIndex = m_fileIndices[socket];
        updateFileStatus(fileIndex, ReceiveStatus::Failed);
        emit fileReceiveFailed(fileIndex, "套接字错误: " + socket->errorString());
    }
}

void ReceiveModel::updateFileStatus(int index, ReceiveStatus status)
{
    if (index >= 0 && index < m_files.size()) {
        m_files[index].status = status;
        
        // 如果完成，记录完成时间
        if (status == ReceiveStatus::Completed) {
            m_files[index].completeTime = QDateTime::currentDateTime();
        }
    }
}

QString ReceiveModel::sanitizeFileName(const QString &originalName)
{
    if (originalName.isEmpty()) {
        return "未命名文件";
    }
    
    // 处理包含路径的文件名
    QString fileName = originalName;
    
    // 将反斜杠转换为正斜杠以统一处理
    fileName.replace('\\', '/');
    
    // 处理绝对路径，移除可能导致安全问题的路径部分
    if (fileName.startsWith('/') || fileName.contains(":/") || fileName.contains("../")) {
        // 移除所有盘符和"../"部分，确保路径是相对路径且不会跳出目标目录
        QRegularExpression unsafePathPattern("^[a-zA-Z]:|\\.\\./");
        fileName.replace(unsafePathPattern, "");
        
        // 移除路径开头的斜杠，确保是相对路径
        while (fileName.startsWith('/')) {
            fileName.remove(0, 1);
        }
    }
    
    // 处理文件名部分中的非法字符
    QStringList pathParts = fileName.split('/');
    for (int i = 0; i < pathParts.size(); ++i) {
        if (pathParts[i].isEmpty()) {
            pathParts[i] = "未命名文件夹";
            continue;
        }
        
        // 去除不允许的字符
        static const QRegularExpression invalidChars("[*?\"<>|:]");
        pathParts[i] = pathParts[i].replace(invalidChars, "_").trimmed();
        
        // 确保每个路径部分不为空且不以点开头
        while (pathParts[i].startsWith(".") && pathParts[i] != "." && pathParts[i] != "..") {
            pathParts[i].remove(0, 1);
        }
        
        if (pathParts[i].isEmpty()) {
            pathParts[i] = "未命名";
        }
    }
    
    // 重新组合路径
    fileName = pathParts.join('/');
    
    // 限制文件名长度（Windows完整路径最大260个字符）
    const int MAX_PATH_LENGTH = 240; // 留一些余量
    if (fileName.length() > MAX_PATH_LENGTH) {
        // 获取最后一个部分（文件名）
        QString lastPart = pathParts.last();
        
        // 保留扩展名
        QString extension;
        int dotIndex = lastPart.lastIndexOf('.');
        if (dotIndex != -1) {
            extension = lastPart.mid(dotIndex);
            lastPart = lastPart.left(dotIndex);
        }
        
        // 截断文件名
        int excessLength = fileName.length() - MAX_PATH_LENGTH;
        lastPart = lastPart.left(lastPart.length() - excessLength - extension.length()) + extension;
        
        // 替换最后一个部分
        pathParts.last() = lastPart;
        fileName = pathParts.join('/');
    }
    
    return fileName;
}
