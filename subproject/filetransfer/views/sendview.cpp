#include "subproject/filetransfer/views/sendview.h"
#include "subproject/filetransfer/utils/urifix.h"
#include "ui_SendView.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>

SendView::SendView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SendView)
{
    ui->setupUi(this);
    setupUI();
}

SendView::~SendView()
{
    delete ui;
}

void SendView::setupUI()
{
    // 设置表格
    ui->tableWidget_Files->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_Files->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_Files->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_Files->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget_Files->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 设置端口范围
    ui->spinBox_Port->setRange(1024, 65535);
    ui->spinBox_Port->setValue(8000);

    // 初始状态
    updateUIState(false);
    setStatusMessage("准备就绪");
}

void SendView::updateFileList(const QList<FileItem> &files)
{
    ui->tableWidget_Files->setRowCount(0);
    
    for (int i = 0; i < files.size(); ++i) {
        const FileItem &item = files[i];
        
        ui->tableWidget_Files->insertRow(i);
        ui->tableWidget_Files->setItem(i, 0, new QTableWidgetItem(item.fileName));
        ui->tableWidget_Files->setItem(i, 1, new QTableWidgetItem(formatFileSize(item.fileSize)));
        ui->tableWidget_Files->setItem(i, 2, new QTableWidgetItem(getFileStatusString(item.status)));
    }
    
    // 添加这行来更新UI状态，确保发送按钮状态正确
    //updateUIState(false);

    bool hasFiles = ui->tableWidget_Files->rowCount() > 0;
    ui->pushButton_Send->setEnabled(!ui->pushButton_Cancel->isEnabled() && hasFiles);
}

void SendView::updateProgress(int fileIndex, qint64 bytesSent, qint64 bytesTotal)
{
    if (fileIndex >= 0 && fileIndex < ui->tableWidget_Files->rowCount()) {
        // 更新文件状态为"已完成"，而不是"发送中..."
        if (bytesSent == bytesTotal) {
            ui->tableWidget_Files->setItem(fileIndex, 2, new QTableWidgetItem("已完成"));
        } else {
            ui->tableWidget_Files->setItem(fileIndex, 2, new QTableWidgetItem("发送中..."));
        }
        
        ui->progressBar->setValue((float)bytesSent/(float)bytesTotal * 100); // 更新进度条

        //qDebug() << "更新进度条" << bytesSent << bytesTotal << bytesSent / bytesTotal * 100;

        // 更新状态信息
        setStatusMessage(QString("已发送: %1 (%2/%3)")
                        .arg(ui->tableWidget_Files->item(fileIndex, 0)->text())
                        .arg(formatFileSize(bytesSent))
                        .arg(formatFileSize(bytesTotal)));
    }
}

void SendView::setStatusMessage(const QString &message)
{
    ui->label_Status->setText(message);
}

void SendView::resetUI()
{
    ui->progressBar->setValue(0);
    setStatusMessage("准备就绪");
    updateUIState(false);
}

QString SendView::getTargetIP() const
{
    return ui->lineEdit_IP->text();
}

int SendView::getTargetPort() const
{
    return ui->spinBox_Port->value();
}

void SendView::updateUIState(bool isTransferActive)
{
    // 传输过程中禁用某些控件
    ui->pushButton_AddFile->setEnabled(!isTransferActive);
    ui->pushButton_RemoveFile->setEnabled(!isTransferActive);
    ui->pushButton_ClearAll->setEnabled(!isTransferActive);
    ui->lineEdit_IP->setEnabled(!isTransferActive);
    ui->spinBox_Port->setEnabled(!isTransferActive);
    ui->pushButton_Send->setEnabled(!isTransferActive && ui->tableWidget_Files->rowCount() > 0);
    ui->pushButton_Cancel->setEnabled(isTransferActive);
}

QString SendView::formatFileSize(qint64 size) const
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

QString SendView::getFileStatusString(FileStatus status) const
{
    switch (status) {
        case FileStatus::Ready:
            return "准备就绪";
        case FileStatus::Sending:
            return "发送中...";
        case FileStatus::Completed:
            return "已完成";
        case FileStatus::Cancelled:
            return "已取消";
        case FileStatus::Failed:
            return "发送失败";
        default:
            return "未知状态";
    }
}

void SendView::on_pushButton_AddFile_clicked()
{
    QStringList filePaths = QFileDialog::getOpenFileNames(
        this,
        "选择要发送的文件",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "所有文件 (*.*)"
    );
    
    if (!filePaths.isEmpty()) {
        emit addFilesRequested(filePaths);
    }
}

void SendView::on_pushButton_RemoveFile_clicked()
{
    int row = ui->tableWidget_Files->currentRow();
    if (row >= 0) {
        emit removeFileRequested(row);
    } else {
        QMessageBox::information(this, "提示", "请先选择要移除的文件");
    }
}

void SendView::on_pushButton_ClearAll_clicked()
{
    if (ui->tableWidget_Files->rowCount() > 0) {
        emit clearFilesRequested();
    }
}

void SendView::on_pushButton_Send_clicked()
{
    if (ui->tableWidget_Files->rowCount() == 0) {
        QMessageBox::information(this, "提示", "请先添加要发送的文件");
        return;
    }
    
    if (getTargetIP().isEmpty()) {
        QMessageBox::warning(this, "错误", "请输入目标IP地址");
        return;
    }
    
    emit sendRequested();
}

void SendView::on_pushButton_Cancel_clicked()
{
    emit cancelRequested();
}

void SendView::setTransferActive(bool active)
{
    updateUIState(active);
}

void SendView::on_pushButton_AddPath_clicked()
{
    selectPath= QFileDialog::getExistingDirectory(
        this, 
        tr("选择文件夹"),
        "", 
        QFileDialog::ShowDirsOnly
    );
    selectPath=URIFix::toPath(selectPath);
    emit addPathRequested(selectPath);
}

