#ifndef SENDMODEL_H
#define SENDMODEL_H

#include <QObject>
#include <QList>
#include <QFileInfo>
#include <QTcpSocket>
#include <QFile>

// 文件传输状态枚举
enum class FileStatus {
    Ready,      // 准备就绪
    Sending,    // 发送中
    Completed,  // 完成
    Cancelled,  // 已取消
    Failed      // 失败
};

// 文件信息结构体
struct FileItem {
    QString filePath;    // 文件完整路径
    QString fileName;    // 文件名
    qint64 fileSize;     // 文件大小
    FileStatus status;   // 文件状态
    qint64 bytesSent;    // 已发送字节数
};

class SendModel : public QObject
{
    Q_OBJECT
public:
    explicit SendModel(QObject *parent = nullptr);
    ~SendModel();

    // 文件操作方法
    bool addFile(const QString &filePath);
    bool removeFile(int index);
    void clearFiles();
    QList<FileItem> getFiles() const;
    int getFileCount() const;

    // 传输设置和控制
    void setTargetAddress(const QString &address);
    void setTargetPort(int port);
    bool startTransfer();
    void cancelTransfer();
    void setParentPath(const QString &path);

signals:
    // 传输状态信号
    void transferStarted();
    void transferProgress(int fileIndex, qint64 bytesSent, qint64 bytesTotal);
    void fileCompleted(int fileIndex);
    void transferCompleted();
    void transferCancelled();
    void transferError(const QString &errorMsg);

private slots:
    void onSocketConnected();
    void onSocketDisconnected();
    void onBytesWritten(qint64 bytes);
    void onSocketError(QAbstractSocket::SocketError socketError);

private:
    QList<FileItem> m_files;        // 文件列表
    QString m_targetAddress;        // 目标IP地址
    int m_targetPort;               // 目标端口
    QTcpSocket *m_socket;           // TCP套接字
    QFile *m_currentFile;           // 当前发送的文件
    int m_currentFileIndex;         // 当前文件索引
    bool m_transferActive;          // 传输活动标志
    qint64 m_bytesSent;             // 当前文件已发送字节数
    qint64 m_headerSize;            // 文件头信息大小
    qint64 m_headerBytesSent;       // 已发送的文件头字节数
    QString parentPath;            // 父路径

    void sendNextFile();            // 发送下一个文件
    void sendFileData();            // 发送文件数据
    void updateFileStatus(int index, FileStatus status); // 更新文件状态
};

#endif // SENDMODEL_H
