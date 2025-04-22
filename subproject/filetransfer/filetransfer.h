#ifndef FILETRANSFER_H
#define FILETRANSFER_H

#include <QMainWindow>
#include "manager/sendmanager.h"
#include "manager/receivemanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class FileTransfer; }
QT_END_NAMESPACE

class FileTransfer : public QMainWindow
{
    Q_OBJECT

public:
    FileTransfer(QWidget *parent = nullptr);
    ~FileTransfer();

private slots:
    void on_actionExit_triggered();
    void on_actionAbout_triggered();

private:
    Ui::FileTransfer *ui;
    SendManager *m_sendManager;
    ReceiveManager *m_receiveManager;
    
    void setupViews();
};
#endif // FILETRANSFER_H
