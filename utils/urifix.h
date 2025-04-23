#ifndef URIFIX_H
#define URIFIX_H

#include <QString>
#include <QUrl>

class URIFix
{
public:
    // 将任何类型的URI转换为Qt可用的路径（所有平台通用）
    static QString toPath(const QString &uri);
    
    // 检查是否是content URI（主要用于Android）
    static bool isContentUri(const QString &uri);
    
    // 将Android content URI转换为文件路径（主要用于Android）
    static QString contentUriToFilePath(const QString &contentUri);
    
    // 将URI解码，处理百分比编码（主要用于Android）
    static QString decodeUri(const QString &uri);
};

#endif // URIFIX_H
