#include "subproject/filetransfer/filetransfer.h"
#include "ui_filetransfer.h"
#include <QMessageBox>
#include <QVBoxLayout>

FileTransfer::FileTransfer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FileTransfer)
    , m_sendManager(nullptr)
    , m_receiveManager(nullptr)
{
    ui->setupUi(this);
    
    // 初始化发送管理器
    m_sendManager = new SendManager(this);
    m_sendManager->init(ui->sendViewContainer);
    
    // 初始化接收管理器
    m_receiveManager = new ReceiveManager(this);
    m_receiveManager->init(ui->receiveViewContainer);
}

FileTransfer::~FileTransfer()
{
    delete ui;
}

void FileTransfer::setupViews()
{
}

void FileTransfer::on_actionExit_triggered()
{
    close();
}

void FileTransfer::on_actionAbout_triggered()
{
    QMessageBox::about(this, "关于文件传输系统", 
                      "文件传输系统 v1.0\n\n"
                      "一个基于Qt的简单文件传输应用程序，支持局域网内文件发送和接收。");
}
