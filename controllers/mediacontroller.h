#ifndef MEDIACONTROLLER_H
#define MEDIACONTROLLER_H

#include <QObject>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QMediaMetaData>
#include "../models/mediamodel.h"

class MediaController : public QObject
{
    Q_OBJECT
public:
    explicit MediaController(MediaModel *model, QObject *parent = nullptr);
    
    // 设置视频输出
    void setVideoOutput(QVideoWidget *videoOutput);
    
    // 播放控制
    void play();
    void pause();
    void stop();
    void setVolume(int volume);
    void setPosition(int seconds);
    void setAutoPlayEnabled(bool enabled);

signals:
    // 转发模型信号给视图
    void metadataChanged(const QMediaMetaData &metaData);
    void durationChanged(qint64 duration);
    void positionChanged(qint64 position);
    void playbackStateChanged(QMediaPlayer::PlaybackState state);
    void hasVideoChanged(bool hasVideo);
    
    // 歌词信号
    void lyricChanged(const QString &currentLyric, const QString &nextLyric);

private:
    MediaModel *m_model;
    
private slots:
    // 接收模型信号并处理
    void handleMetadataChange(const QMediaMetaData &metaData);
    void handleDurationChange(qint64 duration);
    void handlePositionChange(qint64 position);
    void handlePlaybackStateChange(QMediaPlayer::PlaybackState state);
    void handleMediaStatusChange(QMediaPlayer::MediaStatus status);
};

#endif // MEDIACONTROLLER_H
