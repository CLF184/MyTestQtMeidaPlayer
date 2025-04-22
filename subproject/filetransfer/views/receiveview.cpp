#include "receiveview.h"
#include "ui_ReceiveView.h"
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QStandardPaths>
#include <QMessageBox>

ReceiveView::ReceiveView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReceiveView),
    m_isListening(false)
{
    ui->setupUi(this);
    
    // 设置默认值
    ui->lineEdit_ListenIP->setText("0.0.0.0");
    ui->spinBox_ListenPort->setValue(8000);
    ui->lineEdit_SavePath->setText(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
    
    // 设置表格属性
    ui->tableWidget_ReceivedFiles->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget_ReceivedFiles->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_ReceivedFiles->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // 设置进度条初始值
    ui->progressBar->setValue(0);
    
    // 设置初始状态
    resetUI();
}

ReceiveView::~ReceiveView()
{
    delete ui;
}

QString ReceiveView::getListenIP() const
{
    return ui->lineEdit_ListenIP->text();
}

int ReceiveView::getListenPort() const
{
    return ui->spinBox_ListenPort->value();
}

QString ReceiveView::getSavePath() const
{
    return ui->lineEdit_SavePath->text();
}

void ReceiveView::setStatusMessage(const QString &message)
{
    ui->label_ReceiveStatus->setText(message);
}

void ReceiveView::updateProgress(int value)
{
    ui->progressBar->setValue(value);
}

void ReceiveView::resetUI()
{
    ui->progressBar->setValue(0);
    setStatusMessage(m_isListening ? "等待接收文件..." : "未开始监听");
}

void ReceiveView::updateFileList(const QList<ReceivedFileItem> &files)
{
    ui->tableWidget_ReceivedFiles->setRowCount(0);
    
    for (int i = 0; i < files.size(); ++i) {
        const ReceivedFileItem &file = files[i];
        
        ui->tableWidget_ReceivedFiles->insertRow(i);
        
        // 设置文件名
        ui->tableWidget_ReceivedFiles->setItem(i, 0, new QTableWidgetItem(file.fileName));
        
        // 设置文件大小
        ui->tableWidget_ReceivedFiles->setItem(i, 1, new QTableWidgetItem(formatFileSize(file.fileSize)));
        
        // 设置发送者
        ui->tableWidget_ReceivedFiles->setItem(i, 2, new QTableWidgetItem(file.sender));
        
        // 设置状态
        ui->tableWidget_ReceivedFiles->setItem(i, 3, new QTableWidgetItem(statusToString(file.status)));
        
        // 设置完成时间
        QString timeStr = file.completeTime.isValid() ? file.completeTime.toString("yyyy-MM-dd hh:mm:ss") : "";
        ui->tableWidget_ReceivedFiles->setItem(i, 4, new QTableWidgetItem(timeStr));
    }
}

void ReceiveView::setListening(bool listening)
{
    m_isListening = listening;
    
    // 更新按钮文本和状态
    ui->pushButton_StartStop->setText(listening ? "停止监听" : "开始监听");
    ui->pushButton_StartStop->setIcon(QIcon::fromTheme(listening ? "media-playback-stop" : "media-playback-start"));
    
    // 禁用/启用设置控件
    ui->lineEdit_ListenIP->setEnabled(!listening);
    ui->spinBox_ListenPort->setEnabled(!listening);
    
    // 更新状态信息
    setStatusMessage(listening ? "等待接收文件..." : "未开始监听");
}

void ReceiveView::on_pushButton_Browse_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, 
                                                   "选择保存目录", 
                                                   ui->lineEdit_SavePath->text(),
                                                   QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
                                                   
    if (!dir.isEmpty()) {
        ui->lineEdit_SavePath->setText(dir);
        emit savePathChanged(dir);
    }
}

void ReceiveView::on_pushButton_OpenFolder_clicked()
{
    // 只发送信号，不直接处理文件夹打开操作
    emit openFolderRequested();
}

void ReceiveView::on_pushButton_ClearList_clicked()
{
    emit clearFileListRequested();
}

void ReceiveView::on_pushButton_StartStop_clicked()
{
    if (!m_isListening) {
        QString ip = getListenIP();
        int port = getListenPort();
        QString savePath = getSavePath();
        emit startListeningRequested(ip, port, savePath);
    } else {
        emit stopListeningRequested();
    }
}

QString ReceiveView::formatFileSize(qint64 size) const
{
    const qint64 KB = 1024;
    const qint64 MB = 1024 * KB;
    const qint64 GB = 1024 * MB;
    
    if (size < KB) {
        return QString("%1 B").arg(size);
    } else if (size < MB) {
        return QString("%1 KB").arg(size / (double)KB, 0, 'f', 2);
    } else if (size < GB) {
        return QString("%1 MB").arg(size / (double)MB, 0, 'f', 2);
    } else {
        return QString("%1 GB").arg(size / (double)GB, 0, 'f', 2);
    }
}

QString ReceiveView::statusToString(ReceiveStatus status) const
{
    switch (status) {
        case ReceiveStatus::Waiting:
            return "等待接收";
        case ReceiveStatus::Receiving:
            return "接收中";
        case ReceiveStatus::Completed:
            return "已完成";
        case ReceiveStatus::Cancelled:
            return "已取消";
        case ReceiveStatus::Failed:
            return "失败";
        default:
            return "未知";
    }
}

// 添加缺失的显示错误方法
void ReceiveView::showError(const QString &errorMessage)
{
    setStatusMessage("错误: " + errorMessage);
    // 可以考虑弹出错误对话框
    QMessageBox::critical(this, "错误", errorMessage);
}

// 添加显示普通消息方法
void ReceiveView::showMessage(const QString &message)
{
    setStatusMessage(message);
    // 可以考虑使用状态栏或其他可视化方式显示
}

// 添加文件到列表的方法
void ReceiveView::addFile(const QString &fileName, qint64 fileSize)
{
    // 创建新的文件项
    int row = ui->tableWidget_ReceivedFiles->rowCount();
    ui->tableWidget_ReceivedFiles->insertRow(row);
    
    // 设置文件名
    ui->tableWidget_ReceivedFiles->setItem(row, 0, new QTableWidgetItem(fileName));
    
    // 设置文件大小
    ui->tableWidget_ReceivedFiles->setItem(row, 1, new QTableWidgetItem(formatFileSize(fileSize)));
    
    // 设置状态为"接收中"
    ui->tableWidget_ReceivedFiles->setItem(row, 3, new QTableWidgetItem(statusToString(ReceiveStatus::Receiving)));
    
    // 更新UI
    ui->tableWidget_ReceivedFiles->scrollToBottom();
}
