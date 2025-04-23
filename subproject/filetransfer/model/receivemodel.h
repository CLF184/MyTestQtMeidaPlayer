#ifndef RECEIVEMODEL_H
#define RECEIVEMODEL_H

#include <QObject>
#include <QList>
#include <QFileInfo>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QDateTime>
#include <QMap>

// 文件接收状态枚举
enum class ReceiveStatus {
    Waiting,    // 等待接收
    Receiving,  // 接收中
    Completed,  // 完成
    Cancelled,  // 已取消
    Failed      // 失败
};

// 接收文件信息结构体
struct ReceivedFileItem {
    QString fileName;       // 文件名
    QString savePath;       // 保存路径
    QString filePath;       // 完整文件路径
    QString sender;         // 发送者IP
    qint64 fileSize;        // 文件大小
    qint64 bytesReceived;   // 已接收字节数
    ReceiveStatus status;   // 文件状态
    bool completed;         // 是否完成
    QDateTime completeTime; // 完成时间
};

class ReceiveModel : public QObject
{
    Q_OBJECT
public:
    explicit ReceiveModel(QObject *parent = nullptr);
    ~ReceiveModel();

    // 服务器操作方法
    bool startServer(const QString &ip, int port, const QString &savePath);
    void stopServer();
    bool isListening() const;
    
    // 设置保存路径
    void setSavePath(const QString &path);
    QString getSavePath() const;
    
    // 获取接收的文件信息
    QList<ReceivedFileItem> getReceivedFiles() const;
    void clearReceivedFiles();

signals:
    // 服务器状态信号
    void serverStarted();
    void serverStopped();
    void serverError(const QString &errorMsg);
    void errorOccurred(const QString &errorMessage);
    
    // 文件接收信号
    void newConnection(const QString &address);
    void clientConnected(const QString &address);
    void clientDisconnected();
    void fileReceiveStarted(const QString &fileName, qint64 fileSize);
    void fileReceiveProgress(qint64 bytesReceived, qint64 bytesTotal);
    void fileReceiveCompleted(int fileIndex);
    void fileReceiveFailed(int fileIndex, const QString &errorMsg);
    
    // 额外添加兼容信号
    void fileTransferStarted(const QString &fileName, qint64 fileSize);
    void fileTransferProgress(int fileIndex, qint64 bytesReceived, qint64 bytesTotal);
    void fileReceived(const QString &fileName);

private slots:
    void onNewConnection();
    void onReadyRead();
    void onSocketDisconnected();
    void onSocketError(QAbstractSocket::SocketError socketError);

private:
    QTcpServer *m_server;
    QString m_savePath;
    QMap<QTcpSocket*, QFile*> m_currentFiles;
    QMap<QTcpSocket*, qint64> m_fileSizes;
    QMap<QTcpSocket*, qint64> m_bytesReceived;
    QMap<QTcpSocket*, QString> m_fileNames;
    QMap<QTcpSocket*, int> m_fileIndices;
    QMap<QTcpSocket*, bool> m_waitingForHeader;
    QList<ReceivedFileItem> m_files;

    void processFileHeader(QTcpSocket *socket);
    void updateFileStatus(int index, ReceiveStatus status);
    QString sanitizeFileName(const QString &originalName);
};

#endif // RECEIVEMODEL_H
