#include "sendcontroller.h"
#include <QMessageBox>

SendController::SendController(SendModel *model, SendView *view, QObject *parent)
    : QObject(parent),
    m_model(model),
    m_view(view)
{
    // 连接视图信号到控制器槽
    connect(m_view, &SendView::sendRequested, this, &SendController::handleSendRequest);
    connect(m_view, &SendView::cancelRequested, this, &SendController::handleCancelRequest);
    connect(m_view, &SendView::addFilesRequested, this, &SendController::handleAddFiles);
    connect(m_view, &SendView::removeFileRequested, this, &SendController::handleRemoveFile);
    connect(m_view, &SendView::clearFilesRequested, this, &SendController::handleClearFiles);
    connect(m_view, &SendView::addPathRequested,this, &SendController::handleAddPath);

    // 连接模型信号到控制器槽
    connect(m_model, &SendModel::transferStarted, this, &SendController::handleTransferStarted);
    connect(m_model, &SendModel::transferProgress, this, &SendController::handleTransferProgress);
    connect(m_model, &SendModel::fileCompleted, this, &SendController::handleFileCompleted);
    connect(m_model, &SendModel::transferCompleted, this, &SendController::handleTransferCompleted);
    connect(m_model, &SendModel::transferCancelled, this, &SendController::handleTransferCancelled);
    connect(m_model, &SendModel::transferError, this, &SendController::handleTransferError);
    connect(m_model, &SendModel::transferStarted, this, &SendController::handeleTransferActive);
    connect(m_model, &SendModel::transferError, this, &SendController::handleTransferNotActive);
    connect(m_model, &SendModel::transferCancelled, this, &SendController::handleTransferNotActive);
    connect(m_model, &SendModel::transferCompleted, this, &SendController::handleTransferNotActive);

    // 初始化视图
    updateFileList();
}

SendController::~SendController()
{
}

void SendController::handeleTransferActive(){
    m_view->setTransferActive(true);
}

void SendController::handleTransferNotActive(){
    m_view->setTransferActive(false);
}

void SendController::handleSendRequest()
{
    // 设置目标地址和端口
    m_model->setTargetAddress(m_view->getTargetIP());
    m_model->setTargetPort(m_view->getTargetPort());

    // 开始传输
    if (!m_model->startTransfer()) {
        QMessageBox::warning(m_view, "错误", "无法开始传输，请检查文件列表和网络设置");
    }
}

void SendController::handleCancelRequest()
{
    m_model->cancelTransfer();
}

void SendController::handleAddFiles(const QStringList &filePaths)
{
    bool anyAdded = false;
    for (const QString &filePath : filePaths) {
        if (m_model->addFile(filePath)) {
            anyAdded = true;
        }
    }

    if (anyAdded) {
        updateFileList();
    }
}



void SendController::handleRemoveFile(int index)
{
    if (m_model->removeFile(index)) {
        updateFileList();
    }
}

void SendController::handleAddPath(const QString &path)
{
    if (path.isEmpty()) {
        return;
    }

    QStringList filepaths;
    QFuture<void> future = QtConcurrent::run([=, &filepaths]() {
        // 使用QDirIterator进行递归搜索
        // QDirIterator::Subdirectories 标志使迭代器递归进入子目录
        QDirIterator it(path, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QString filePath = it.next();
            QFileInfo fileInfo(filePath);
            // 只检查文件
            if (fileInfo.isFile()) {
                filepaths.append(fileInfo.filePath());
                qDebug() << "找到文件:" << fileInfo.filePath();
            }
        }
    });

    future.waitForFinished();

    if (filepaths.isEmpty()) {
        qDebug() << "在目录" << path << "及其子目录中没有找到文件";
    } else {
        qDebug() << "共找到" << filepaths.size() << "个文件";
    }

    bool anyAdded = false;
    for (const QString &path : filepaths) {
        if(m_model->addFile(path)){
            anyAdded = true;
        }
    }
    if (anyAdded) {
        m_model->setParentPath(path);
        updateFileList();
    }
}

void SendController::handleClearFiles()
{
    m_model->clearFiles();
    updateFileList();
}

void SendController::handleTransferStarted()
{
    m_view->setStatusMessage("开始传输文件...");
    m_view->setTransferActive(true);
}

void SendController::handleTransferProgress(int fileIndex, qint64 bytesSent, qint64 bytesTotal)
{
    m_view->updateProgress(fileIndex, bytesSent, bytesTotal);
    //updateFileList();
}

void SendController::handleFileCompleted(int fileIndex)
{
    QList<FileItem> files = m_model->getFiles();
    if (fileIndex >= 0 && fileIndex < files.size()) {
        QString fileName = files[fileIndex].fileName;
        m_view->setStatusMessage(QString("文件 %1 发送完成").arg(fileName));
    }
    updateFileList();
}

void SendController::handleTransferCompleted()
{
    m_view->setStatusMessage("所有文件传输完成");
    QMessageBox::information(m_view, "传输完成", "所有文件已成功传输");
    m_view->resetUI();
    updateFileList();
}

void SendController::handleTransferCancelled()
{
    m_view->setStatusMessage("传输已取消");
    m_view->resetUI();
    updateFileList();
}

void SendController::handleTransferError(const QString &errorMsg)
{
    m_view->setStatusMessage("传输错误: " + errorMsg);
    QMessageBox::critical(m_view, "传输错误", "发送文件时出错: " + errorMsg);
    m_view->resetUI();
    updateFileList();
}

void SendController::updateFileList()
{
    m_view->updateFileList(m_model->getFiles());
}
