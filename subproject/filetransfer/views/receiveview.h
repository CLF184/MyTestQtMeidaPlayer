#ifndef RECEIVEVIEW_H
#define RECEIVEVIEW_H

#include <QWidget>
#include "../model/receivemodel.h"

namespace Ui {
class ReceiveView;
}

class ReceiveView : public QWidget
{
    Q_OBJECT

public:
    explicit ReceiveView(QWidget *parent = nullptr);
    ~ReceiveView();

    // 获取UI输入值
    QString getListenIP() const;
    int getListenPort() const;
    QString getSavePath() const;

    // UI更新方法
    void setStatusMessage(const QString &message);
    void showError(const QString &errorMessage);
    void showMessage(const QString &message);
    void updateProgress(int value);
    void resetUI();
    void updateFileList(const QList<ReceivedFileItem> &files);
    void addFile(const QString &fileName, qint64 fileSize);
    void setListening(bool listening);

signals:
    // 添加必要的信号
    void startListeningRequested(const QString &ip, int port, const QString &savePath);
    void stopListeningRequested();
    void openFolderRequested();
    void clearFileListRequested();
    void savePathChanged(const QString &path);

private slots:
    void on_pushButton_Browse_clicked();
    // 添加缺失的槽函数声明
    void on_pushButton_OpenFolder_clicked();
    void on_pushButton_ClearList_clicked();
    void on_pushButton_StartStop_clicked();

private:
    Ui::ReceiveView *ui;
    bool m_isListening;

    // 辅助方法
    QString formatFileSize(qint64 size) const;
    QString statusToString(ReceiveStatus status) const;
};

#endif // RECEIVEVIEW_H
