#include "lyricmodel.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <algorithm>

LyricModel::LyricModel(QObject *parent)
    : QObject(parent),
      m_loaded(false)
{
}

bool LyricModel::loadFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开歌词文件:" << filePath;
        return false;
    }
    
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();
    
    return loadFromText(content);
}

bool LyricModel::loadFromText(const QString &lyricText)
{
    if (lyricText.isEmpty())
        return false;
    
    clear();
    
    // 正则表达式匹配时间标签 [mm:ss.xx]
    QRegularExpression timeTagRegex("\\[(\\d{2}):(\\d{2})\\.(\\d{2})\\]");
    
    // 按行解析歌词
    QStringList lines = lyricText.split('\n');
    for (const QString &line : lines) {
        QString trimmedLine = line.trimmed();
        if (trimmedLine.isEmpty())
            continue;
        
        // 检查是否包含元数据标签如 [ti:歌名]
        if (trimmedLine.startsWith("[ti:")) {
            m_title = trimmedLine.mid(4, trimmedLine.length() - 5);
            continue;
        }
        else if (trimmedLine.startsWith("[ar:")) {
            m_artist = trimmedLine.mid(4, trimmedLine.length() - 5);
            continue;
        }
        else if (trimmedLine.startsWith("[al:")) {
            m_album = trimmedLine.mid(4, trimmedLine.length() - 5);
            continue;
        }
        
        // 匹配时间标签
        QRegularExpressionMatchIterator matches = timeTagRegex.globalMatch(trimmedLine);
        QString content;
        QList<qint64> timestamps;
        
        // 提取内容部分（不含时间标签）
        int lastPos = 0;
        while (matches.hasNext()) {
            QRegularExpressionMatch match = matches.next();
            lastPos = qMax(lastPos, match.capturedEnd());
            
            // 将时间标签转换为毫秒
            qint64 time = parseTimeTag(match.captured(0));
            if (time >= 0)
                timestamps.append(time);
        }
        
        // 提取歌词内容部分
        if (lastPos < trimmedLine.length())
            content = trimmedLine.mid(lastPos).trimmed();
        
        // 为每个时间标签创建一个歌词行
        for (qint64 time : timestamps) {
            LyricLine lyricLine;
            lyricLine.time = time;
            lyricLine.content = content;
            m_lyrics.append(lyricLine);
        }
    }
    
    // 按时间排序
    std::sort(m_lyrics.begin(), m_lyrics.end(), 
        [](const LyricLine &a, const LyricLine &b) { return a.time < b.time; });
    
    m_loaded = !m_lyrics.isEmpty();
    return m_loaded;
}

void LyricModel::clear()
{
    m_lyrics.clear();
    m_title.clear();
    m_artist.clear();
    m_album.clear();
    m_loaded = false;
}

QString LyricModel::getLyricByTime(qint64 time)
{
    if (m_lyrics.isEmpty())
        return QString();
    
    // 二分查找最接近的歌词
    int left = 0;
    int right = m_lyrics.size() - 1;
    
    // 如果在第一行歌词时间之前
    if (time < m_lyrics.first().time)
        return QString();
    
    // 如果在最后一行歌词时间之后
    if (time >= m_lyrics.last().time)
        return m_lyrics.last().content;
    
    // 二分查找当前时间对应的歌词
    while (left <= right) {
        int mid = left + (right - left) / 2;
        
        if (mid < m_lyrics.size() - 1 && 
            time >= m_lyrics[mid].time && 
            time < m_lyrics[mid + 1].time) {
            return m_lyrics[mid].content;
        }
        
        if (time < m_lyrics[mid].time) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }
    
    return QString();
}

QPair<QString, QString> LyricModel::getCurrentAndNextLyric(qint64 time)
{
    if (m_lyrics.isEmpty())
        return qMakePair(QString(), QString());
    
    // 寻找当前歌词
    QString currentLyric;
    QString nextLyric;
    
    // 如果在第一行歌词时间之前
    if (time < m_lyrics.first().time) {
        currentLyric = QString();
        nextLyric = m_lyrics.first().content;
        return qMakePair(currentLyric, nextLyric);
    }
    
    // 如果在最后一行歌词时间之后
    if (time >= m_lyrics.last().time) {
        currentLyric = m_lyrics.last().content;
        nextLyric = QString();
        return qMakePair(currentLyric, nextLyric);
    }
    
    // 查找当前歌词位置
    int index = 0;
    for (int i = 0; i < m_lyrics.size(); i++) {
        if (i < m_lyrics.size() - 1 && 
            time >= m_lyrics[i].time && 
            time < m_lyrics[i + 1].time) {
            index = i;
            break;
        }
    }
    
    currentLyric = m_lyrics[index].content;
    if (index < m_lyrics.size() - 1) {
        nextLyric = m_lyrics[index + 1].content;
    }
    
    return qMakePair(currentLyric, nextLyric);
}

QList<LyricLine> LyricModel::getAllLyrics() const
{
    return m_lyrics;
}

bool LyricModel::isLoaded() const
{
    return m_loaded;
}

qint64 LyricModel::parseTimeTag(const QString &timeTag)
{
    // 提取 [mm:ss.xx] 格式的时间
    QRegularExpression regex("\\[(\\d{2}):(\\d{2})\\.(\\d{2})\\]");
    QRegularExpressionMatch match = regex.match(timeTag);
    
    if (match.hasMatch()) {
        int minutes = match.captured(1).toInt();
        int seconds = match.captured(2).toInt();
        int milliseconds = match.captured(3).toInt() * 10; // 转换为毫秒
        
        return minutes * 60 * 1000 + seconds * 1000 + milliseconds;
    }
    
    return -1;
}

QString LyricModel::findLyricFile(const QString &mediaPath, const QString &title, const QString &artist)
{
    if (mediaPath.isEmpty())
        return QString();
    
    QFileInfo mediaFileInfo(mediaPath);
    QString baseName = mediaFileInfo.completeBaseName();
    QString dirPath = mediaFileInfo.dir().path();

    qDebug()<<"查找歌词文件:" << dirPath;
    qDebug()<<"标题:" << title;
    qDebug()<<"艺术家:" << artist;
    
    // 先尝试同名的.lrc文件
    QString lrcPath = dirPath + "/" + baseName + ".lrc";
    if (QFileInfo::exists(lrcPath)) {
        return lrcPath;
    }

    // 尝试艺术家-标题命名的歌词文件
    if (!artist.isEmpty() && !title.isEmpty()) {
        lrcPath = dirPath + "/" + artist + " - " + title + ".lrc";
        if (QFileInfo::exists(lrcPath)) {
            return lrcPath;
        }
    }

    // 尝试艺术家-文件名命名的歌词文件
    if (!artist.isEmpty()) {
        lrcPath = dirPath + "/" + artist + " - " + baseName + ".lrc";
        if (QFileInfo::exists(lrcPath)) {
            return lrcPath;
        }
    }
    
    // 再尝试同目录下所有.lrc文件，查找文件名包含媒体文件名的歌词文件
    QDir dir(dirPath);
    QStringList filters;
    filters << "*.lrc";
    dir.setNameFilters(filters);
    
    QStringList lrcFiles = dir.entryList();
    for (const QString &lrcFile : lrcFiles) {
        if (lrcFile.contains(title, Qt::CaseInsensitive)) {
            return dirPath + "/" + lrcFile;
        }
    }
    
    return QString();
}
