#include "receivecontroller.h"
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>

ReceiveController::ReceiveController(ReceiveModel *model, ReceiveView *view, QObject *parent)
    : QObject(parent), m_model(model), m_view(view)
{
    // 连接视图信号 - 确保这些信号在ReceiveView中已正确声明
    connect(m_view, &ReceiveView::startListeningRequested, this, &ReceiveController::handleStartListening);
    connect(m_view, &ReceiveView::stopListeningRequested, this, &ReceiveController::handleStopListening);
    connect(m_view, &ReceiveView::clearFileListRequested, this, &ReceiveController::handleClearFileList);
    connect(m_view, &ReceiveView::savePathChanged, this, &ReceiveController::handleSavePathChanged);
    connect(m_view, &ReceiveView::openFolderRequested, this, &ReceiveController::handleOpenFolder);
    
    // 连接模型信号
    connect(m_model, &ReceiveModel::serverStarted, this, &ReceiveController::handleServerStarted);
    connect(m_model, &ReceiveModel::serverStopped, this, &ReceiveController::handleServerStopped);
    connect(m_model, &ReceiveModel::serverError, this, &ReceiveController::handleServerError);
    connect(m_model, &ReceiveModel::newConnection, this, &ReceiveController::handleNewConnection);
    connect(m_model, &ReceiveModel::fileReceiveStarted, this, &ReceiveController::handleFileReceiveStarted);
    connect(m_model, &ReceiveModel::fileReceiveProgress, this, &ReceiveController::handleFileReceiveProgress);
    connect(m_model, &ReceiveModel::fileReceiveCompleted, this, &ReceiveController::handleFileReceiveCompleted);
    connect(m_model, &ReceiveModel::fileReceiveFailed, this, &ReceiveController::handleFileReceiveFailed);
}

ReceiveController::~ReceiveController()
{
}

// 文件名清理
QString ReceiveController::sanitizeFileName(const QString &originalName)
{
    if (originalName.isEmpty())
        return "unnamed_file";
    
    // 删除不允许的Windows文件名字符 \ / : * ? " < > |
    QString cleanName = originalName;
    cleanName.replace(QRegularExpression("[\\\\/:*?\"<>|]"), "_");
    
    // 文件名不应以点或空格开始或结束
    cleanName = cleanName.trimmed();
    if (cleanName.startsWith('.'))
        cleanName.prepend("_");
    
    // 检查文件名长度
    if (cleanName.length() > 255)
        cleanName = cleanName.left(255);
    
    return cleanName;
}

bool ReceiveController::isValidFilePath(const QString &path)
{
    // 检查路径是否为空
    if (path.isEmpty())
        return false;
    
    // 检查路径是否太长 (Windows MAX_PATH = 260)
    if (path.length() > 260)
        return false;
    
    QFileInfo info(path);
    return info.isWritable();
}

bool ReceiveController::ensureSaveDirectoryExists(const QString &dirPath)
{
    QDir dir(dirPath);
    if (!dir.exists()) {
        return dir.mkpath(".");
    }
    return QFileInfo(dirPath).isWritable();
}

bool ReceiveController::isValidDataSize(qint64 size)
{
    // 验证数据大小参数
    // 1KB = 1024字节, 1MB = 1024KB, 1GB = 1024MB
    const qint64 MAX_FILE_SIZE = 4LL * 1024 * 1024 * 1024; // 限制最大4GB
    
    return (size > 0 && size <= MAX_FILE_SIZE); // 文件大小必须为正数且不超过上限
}

void ReceiveController::handleReceiveError(const QString &errorType, const QString &details)
{
    QString errorMsg = errorType + ": " + details;
    m_view->showError(errorMsg);
    qWarning() << errorMsg;
}

// 处理启动监听请求
void ReceiveController::handleStartListening(const QString &ip, int port, const QString &savePath)
{
    if (!ensureSaveDirectoryExists(savePath)) {
        m_view->showError("保存路径无效或无法访问: " + savePath);
        return;
    }
    
    if (!m_model->startServer(ip, port, savePath)) {
        m_view->showError("无法启动服务器，请检查IP和端口设置");
    }
}

void ReceiveController::handleStopListening()
{
    m_model->stopServer();
}

void ReceiveController::handleClearFileList()
{
    m_model->clearReceivedFiles();
    updateFileList();
}

void ReceiveController::handleSavePathChanged(const QString &path)
{
    m_model->setSavePath(path);
}

void ReceiveController::handleServerStarted()
{
    m_view->setListening(true);
    m_view->setStatusMessage("监听已启动，等待连接...");
}

void ReceiveController::handleServerStopped()
{
    m_view->setListening(false);
    m_view->setStatusMessage("监听已停止");
}

void ReceiveController::handleServerError(const QString &errorMsg)
{
    m_view->setListening(false);
    m_view->setStatusMessage("服务器错误: " + errorMsg);
    QMessageBox::critical(m_view, "服务器错误", "启动监听时出错: " + errorMsg);
}

void ReceiveController::handleNewConnection(const QString &address)
{
    m_view->setStatusMessage("新连接来自: " + address);
}

// 处理文件接收开始事件
void ReceiveController::handleFileReceiveStarted(const QString &fileName, qint64 fileSize)
{
    QString cleanFileName = sanitizeFileName(fileName);
    
    if (cleanFileName != fileName) {
        m_view->showMessage("文件名已被修正: " + fileName + " -> " + cleanFileName);
    }
    
    if (!isValidDataSize(fileSize)) {
        handleReceiveError("无效文件大小", QString::number(fileSize));
        m_view->showError("接收到无效的文件大小: " + QString::number(fileSize) + "，传输已取消");
        return;
    }
    
    m_view->addFile(cleanFileName, fileSize);
}

// 处理文件接收进度，不再显示进度更新
void ReceiveController::handleFileReceiveProgress(int fileIndex, qint64 bytesReceived, qint64 bytesTotal)
{
    Q_UNUSED(fileIndex);
    Q_UNUSED(bytesReceived);
    Q_UNUSED(bytesTotal);
    
    // 不再更新进度条和文件列表
    // 接收完成后会通过handleFileReceiveCompleted处理
}

void ReceiveController::handleFileReceiveCompleted(int fileIndex)
{
    QList<ReceivedFileItem> files = m_model->getReceivedFiles();
    if (fileIndex >= 0 && fileIndex < files.size()) {
        QString fileName = files[fileIndex].fileName;
        m_view->setStatusMessage(QString("文件 %1 接收完成").arg(fileName));
    }
    
    // 完成后设置进度为100%
    m_view->updateProgress(100);
    updateFileList();
}

void ReceiveController::handleFileReceiveFailed(int fileIndex, const QString &errorMsg)
{
    QList<ReceivedFileItem> files = m_model->getReceivedFiles();
    QString fileName = (fileIndex >= 0 && fileIndex < files.size()) ? files[fileIndex].fileName : "未知文件";
    
    m_view->setStatusMessage(QString("文件 %1 接收失败: %2").arg(fileName, errorMsg));
    QMessageBox::warning(m_view, "接收错误", QString("接收文件 %1 失败: %2").arg(fileName, errorMsg));
    
    updateFileList();
}

// 确保updateFileList()方法与ReceiveModel的getReceivedFiles()方法返回类型匹配
void ReceiveController::updateFileList()
{
    m_view->updateFileList(m_model->getReceivedFiles());
}

void ReceiveController::handleOpenFolder()
{
    // 由控制器处理打开文件夹的操作
    QString savePath = m_model->getSavePath();
    if (!savePath.isEmpty()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(savePath));
    } else {
        m_view->showError("保存路径未设置或无效");
    }
}
