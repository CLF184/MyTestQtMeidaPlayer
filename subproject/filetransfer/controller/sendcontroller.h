#ifndef SENDCONTROLLER_H
#define SENDCONTROLLER_H

#include <QObject>
#include <QDirIterator>
#include <QFileInfo>
#include <QtConcurrent>
#include "../model/sendmodel.h"
#include "../views/sendview.h"

class SendController : public QObject
{
    Q_OBJECT
public:
    explicit SendController(SendModel *model, SendView *view, QObject *parent = nullptr);
    ~SendController();

private slots:
    // 处理来自视图的事件
    void handleSendRequest();
    void handleCancelRequest();
    void handleAddFiles(const QStringList &filePaths);
    void handleRemoveFile(int index);
    void handleClearFiles();
    
    // 处理来自模型的事件
    void handleTransferStarted();
    void handleTransferProgress(int fileIndex, qint64 bytesSent, qint64 bytesTotal);
    void handleFileCompleted(int fileIndex);
    void handleTransferCompleted();
    void handleTransferCancelled();
    void handleTransferError(const QString &errorMsg);
    void handleAddPath(const QString &path);

private:
    SendModel *m_model;
    SendView *m_view;
    
    void updateFileList();
};

#endif // SENDCONTROLLER_H
