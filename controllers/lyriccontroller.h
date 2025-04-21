#ifndef LYRICCONTROLLER_H
#define LYRICCONTROLLER_H

#include <QObject>
#include <QMediaPlayer>
#include "../models/lyricmodel.h"
#include "../views/lyricview.h"

class LyricController : public QObject
{
    Q_OBJECT
public:
    explicit LyricController(QObject *parent = nullptr);
    
    // 设置模型和视图
    void setModel(LyricModel *model);
    void setView(LyricView *view);
    void setMediaPlayer(QMediaPlayer *player);
    
    // 加载歌词
    bool loadLyricForMedia(const QString &mediaPath,const QMediaMetaData &metadata);
    
public slots:
    // 响应媒体播放位置变化
    void onPositionChanged(qint64 position);
    
    // 删除不再使用的槽函数
    // void onMediaChanged(const QUrl &media);
    
    // 添加新的槽函数，响应媒体路径变化
    void onLyricChanged(const QString &path,const QMediaMetaData &metadata);
    
private:
    LyricModel *m_model;
    LyricView *m_view;
    //QMediaPlayer *m_player;
};

#endif // LYRICCONTROLLER_H
