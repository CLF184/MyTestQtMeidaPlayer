#ifndef SENDVIEW_H
#define SENDVIEW_H

#include <QWidget>
#include <QFileInfo>
#include <QList>
#include <QString>
#include "../model/sendmodel.h"

namespace Ui {
class SendView;
}

class SendView : public QWidget
{
    Q_OBJECT

public:
    explicit SendView(QWidget *parent = nullptr);
    ~SendView();

    // 更新界面
    void updateFileList(const QList<FileItem> &files);
    void updateProgress(int fileIndex, qint64 bytesSent, qint64 bytesTotal);
    void setStatusMessage(const QString &message);
    void resetUI();
    void setTransferActive(bool active); // 新增公共方法

    // 获取用户输入
    QString getTargetIP() const;
    int getTargetPort() const;

signals:
    // 用户操作信号
    void sendRequested();
    void cancelRequested();
    void addFilesRequested(const QStringList &filePaths);
    void removeFileRequested(int index);
    void clearFilesRequested();
    void addPathRequested(const QString &path); // 新增信号

private slots:
    // UI事件处理
    void on_pushButton_AddFile_clicked();
    void on_pushButton_RemoveFile_clicked();
    void on_pushButton_ClearAll_clicked();
    void on_pushButton_Send_clicked();
    void on_pushButton_Cancel_clicked();
    void on_pushButton_AddPath_clicked();

private:
    Ui::SendView *ui;

    // 辅助方法
    void setupUI();
    void updateUIState(bool isTransferActive);
    QString formatFileSize(qint64 size) const;
    QString getFileStatusString(FileStatus status) const;
    QString selectPath;
};

#endif // SENDVIEW_H
