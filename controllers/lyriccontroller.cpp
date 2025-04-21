#include "lyriccontroller.h"
#include <QMediaMetaData>
#include <QUrl>
#include <QDebug>

LyricController::LyricController(QObject *parent)
    : QObject(parent),
      m_model(nullptr),
      m_view(nullptr)
{
}

void LyricController::setModel(LyricModel *model)
{
    m_model = model;
}

void LyricController::setView(LyricView *view)
{
    m_view = view;
}

bool LyricController::loadLyricForMedia(const QString &mediaPath,const QMediaMetaData &metadata)
{
    if (!m_model) return false;
    
    // 清除之前的歌词
    m_model->clear();
    
    // if (mediaPath.isEmpty()) {
    //     if (m_view) {
    //         m_view->updateLyric("", "");
    //     }
    //     return false;
    // }
    
    // 获取媒体元数据
    QString title = metadata.value(QMediaMetaData::Title).toString();
    QString artist = metadata.value(QMediaMetaData::ContributingArtist).toString();
    
    // 查找歌词文件
    QString lyricPath = LyricModel::findLyricFile(mediaPath, title, artist);
    
    if (lyricPath.isEmpty()) {
        qDebug() << "找不到歌词文件";
        if (m_view) {
            m_view->updateLyric("找不到歌词", "");
        }
        return false;
    }
    
    // 加载歌词文件
    bool success = m_model->loadFromFile(lyricPath);
    
    if (!success && m_view) {
        m_view->updateLyric("歌词加载失败", "");
    }
    
    return success;
}

void LyricController::onPositionChanged(qint64 position)
{
    if (!m_model || !m_view) return;
    
    if (m_model->isLoaded()) {
        QPair<QString, QString> lyrics = m_model->getCurrentAndNextLyric(position);
        m_view->updateLyric(lyrics.first, lyrics.second);
    }
}

// 删除不再使用的槽函数实现
/*
void LyricController::onMediaChanged(const QUrl &media)
{
    if (media.isEmpty()) return;
    
    QString localFile = media.toLocalFile();
    if (!localFile.isEmpty()) {
        loadLyricForMedia(localFile);
    }
}
*/

void LyricController::onLyricChanged(const QString &path,const QMediaMetaData &metadata)
{

    // 当媒体路径变化时加载对应的歌词    
    loadLyricForMedia(path,metadata);
}
