#ifndef LYRICPARSER_H
#define LYRICPARSER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QStringList>

// 歌词行结构体
struct LyricLine {
    qint64 time;       // 时间戳（毫秒）
    QString content;   // 歌词内容
};

class LyricParser : public QObject
{
    Q_OBJECT
public:
    explicit LyricParser(QObject *parent = nullptr);
    
    // 从文件加载歌词
    bool loadFromFile(const QString &filePath);
    
    // 从文本加载歌词
    bool loadFromText(const QString &lyricText);
    
    // 清空当前歌词
    void clear();
    
    // 根据时间获取当前歌词
    QString getLyricByTime(qint64 time);
    
    // 获取当前行和下一行歌词
    QPair<QString, QString> getCurrentAndNextLyric(qint64 time);
    
    // 获取所有歌词行
    QList<LyricLine> getAllLyrics() const;
    
    // 歌词是否已加载
    bool isLoaded() const;
    
private:
    // 解析时间标签, 格式如 [00:01.02]
    qint64 parseTimeTag(const QString &timeTag);
    
    // 按时间顺序排序的歌词集合
    QList<LyricLine> m_lyrics;
    
    // 歌词元数据
    QString m_title;
    QString m_artist;
    QString m_album;
    
    bool m_loaded;
};

#endif // LYRICPARSER_H
