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
    explicit MediaController(MediaModel *mediaModel, QObject *parent = nullptr);
    
    // 播放控制
    void play();
    void pause();
    void stop();
    void setVolume(int volume);
    void setPosition(int position);
    void setVideoOutput(QVideoWidget *videoWidget);
    void setAutoPlayEnabled(bool enabled);

signals:
    // 转发模型的信号给视图
    void metadataChanged(const QMediaMetaData &metaData);
    void durationChanged(qint64 duration);
    void positionChanged(qint64 position);
    void playbackStateChanged(QMediaPlayer::PlaybackState state);
    void hasVideoChanged(bool hasVideo);

private slots:
    // 处理模型信号
    void onMetadataChanged(const QMediaMetaData &metaData);
    void onDurationChanged(qint64 duration);
    void onPositionChanged(qint64 position);
    void onPlaybackStateChanged(QMediaPlayer::PlaybackState state);
    void onHasVideoChanged(bool hasVideo);

private:
    MediaModel *m_mediaModel;
};

#endif // MEDIACONTROLLER_H
