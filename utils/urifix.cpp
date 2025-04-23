#include "urifix.h"
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QRegularExpression>

QString URIFix::toPath(const QString &uri)
{
    if (uri.isEmpty()) {
        return QString();
    }
    
#ifdef Q_OS_ANDROID
    // 只在Android平台处理content URI
    if (isContentUri(uri)) {
        return contentUriToFilePath(uri);
    }
#endif

    // 对于常规URI，使用QUrl处理
    QUrl url(uri);
    if (url.isLocalFile()) {
        return url.toLocalFile();
    }
    
    // 如果不是特殊URI，直接返回
    return uri;
}

bool URIFix::isContentUri(const QString &uri)
{
#ifdef Q_OS_ANDROID
    return uri.startsWith("content://");
#else
    Q_UNUSED(uri);
    return false;
#endif
}

QString URIFix::contentUriToFilePath(const QString &contentUri)
{
#ifdef Q_OS_ANDROID
    QRegularExpression regex("content://[^/]+/tree/primary(?:%3A|:)(.+)");
    QRegularExpressionMatch match = regex.match(contentUri);
    
    if (match.hasMatch()) {
        QString relativePath = decodeUri(match.captured(1));
        
        // 使用ANDROID外部存储根目录，通常是"/storage/emulated/0/"
        QString externalStorage = "/storage/emulated/0";
        // 或者尝试使用环境变量
        // QString externalStorage = QProcessEnvironment::systemEnvironment().value("EXTERNAL_STORAGE", "/storage/emulated/0");
        
        return QDir(externalStorage).filePath(relativePath);
    }
#endif
    return contentUri;
}

QString URIFix::decodeUri(const QString &uri)
{
#ifdef Q_OS_ANDROID
    QString result = uri;
    
    // 处理常见的URL编码
    result.replace("%3A", ":");
    result.replace("%2F", "/");
    result.replace("%20", " ");
    
    // 使用QUrl来完全解码
    return QUrl::fromPercentEncoding(result.toUtf8());
#else
    return uri;
#endif
}
