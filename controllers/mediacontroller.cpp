#include "mediacontroller.h"

MediaController::MediaController(MediaModel *model, QObject *parent)
    : QObject(parent),
      m_model(model)
{
    // 连接模型的信号到控制器的槽
    if (m_model) {
        connect(m_model, &MediaModel::metadataChanged, this, &MediaController::handleMetadataChange);
        connect(m_model, &MediaModel::durationChanged, this, &MediaController::handleDurationChange);
        connect(m_model, &MediaModel::positionChanged, this, &MediaController::handlePositionChange);
        connect(m_model, &MediaModel::playbackStateChanged, this, &MediaController::handlePlaybackStateChange);
        connect(m_model, &MediaModel::lyricChanged, this, &MediaController::lyricChanged);
    }
}

void MediaController::setVideoOutput(QVideoWidget *videoOutput)
{
    if (m_model) {
        m_model->getPlayer()->setVideoOutput(videoOutput);
    }
}

void MediaController::play()
{
    if (m_model) {
        m_model->play();
    }
}

void MediaController::pause()
{
    if (m_model) {
        m_model->pause();
    }
}

void MediaController::stop()
{
    if (m_model) {
        m_model->stop();
    }
}

void MediaController::setVolume(int volume)
{
    if (m_model) {
        m_model->setVolume(volume);
    }
}

void MediaController::setPosition(int seconds)
{
    if (m_model) {
        m_model->setPosition(seconds);
    }
}

void MediaController::setAutoPlayEnabled(bool enabled)
{
    if (m_model) {
        m_model->setAutoPlayEnabled(enabled);
    }
}

void MediaController::handleMetadataChange(const QMediaMetaData &metaData)
{
    emit metadataChanged(metaData);
    
    // 视频状态变化 (使用 hasVideo() 函数)
    bool hasVideo = m_model->getPlayer()->hasVideo();
    emit hasVideoChanged(hasVideo);
}

void MediaController::handleDurationChange(qint64 duration)
{
    emit durationChanged(duration);
}

void MediaController::handlePositionChange(qint64 position)
{
    emit positionChanged(position);
}

void MediaController::handlePlaybackStateChange(QMediaPlayer::PlaybackState state)
{
    emit playbackStateChanged(state);
}

void MediaController::handleMediaStatusChange(QMediaPlayer::MediaStatus status)
{
    // 可以根据需要处理媒体状态变化
}
