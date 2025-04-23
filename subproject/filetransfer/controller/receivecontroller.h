#ifndef RECEIVECONTROLLER_H
#define RECEIVECONTROLLER_H

#include <QObject>
#include <QRegularExpression>
#include <QDir>
#include <QFileInfo>
#include "../model/receivemodel.h"
#include "../views/receiveview.h"

class ReceiveController : public QObject
{
    Q_OBJECT
public:
    explicit ReceiveController(ReceiveModel *model, ReceiveView *view, QObject *parent = nullptr);
    ~ReceiveController();

private slots:
    // 处理来自视图的事件
    void handleStartListening(const QString &ip, int port, const QString &savePath);
    void handleStopListening();
    void handleOpenFolder();
    void handleClearFileList();
    void handleSavePathChanged(const QString &path);
    
    // 处理来自模型的事件
    void handleServerStarted();
    void handleServerStopped();
    void handleServerError(const QString &errorMsg);
    void handleNewConnection(const QString &address);
    void handleFileReceiveStarted(const QString &fileName, qint64 fileSize);
    void handleFileReceiveProgress(qint64 bytesReceived, qint64 bytesTotal);
    void handleFileReceiveCompleted(int fileIndex);
    void handleFileReceiveFailed(int fileIndex, const QString &errorMsg);

private:
    ReceiveModel *m_model;
    ReceiveView *m_view;
    
    void updateFileList();
    
    // 添加文件名验证方法
    QString sanitizeFileName(const QString &originalName);
    
    // 检查文件路径合法性
    bool isValidFilePath(const QString &path);
    
    // 确保保存目录存在
    bool ensureSaveDirectoryExists(const QString &dirPath);
    
    // 添加数据包大小验证方法
    bool isValidDataSize(qint64 size);
    
    // 处理文件接收错误
    void handleReceiveError(const QString &errorType, const QString &details);
};

#endif // RECEIVECONTROLLER_H
