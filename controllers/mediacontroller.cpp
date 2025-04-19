#include "mediacontroller.h"

MediaController::MediaController(MediaModel *mediaModel, QObject *parent)
    : QObject(parent),
      m_mediaModel(mediaModel)
{
    if (m_mediaModel) {
        // 连接模型信号到控制器槽
        connect(m_mediaModel, &MediaModel::metadataChanged, this, &MediaController::onMetadataChanged);
        connect(m_mediaModel, &MediaModel::durationChanged, this, &MediaController::onDurationChanged);
        connect(m_mediaModel, &MediaModel::positionChanged, this, &MediaController::onPositionChanged);
        connect(m_mediaModel, &MediaModel::playbackStateChanged, this, &MediaController::onPlaybackStateChanged);
        connect(m_mediaModel, &MediaModel::hasVideoChanged, this, &MediaController::onHasVideoChanged);
    }
}

void MediaController::play()
{
    if (m_mediaModel) {
        m_mediaModel->play();
    }
}

void MediaController::pause()
{
    if (m_mediaModel) {
        m_mediaModel->pause();
    }
}

void MediaController::stop()
{
    if (m_mediaModel) {
        m_mediaModel->stop();
    }
}

void MediaController::setVolume(int volume)
{
    if (m_mediaModel) {
        m_mediaModel->setVolume(volume);
    }
}

void MediaController::setPosition(int position)
{
    if (m_mediaModel) {
        m_mediaModel->setPosition(position);
    }
}

void MediaController::setVideoOutput(QVideoWidget *videoWidget)
{
    if (m_mediaModel) {
        mymediaplayer *player = m_mediaModel->getPlayer();
        if (player) {
            player->setVideoOutput(videoWidget);
        }
    }
}

void MediaController::setAutoPlayEnabled(bool enabled)
{
    if (m_mediaModel) {
        m_mediaModel->setAutoPlayEnabled(enabled);
    }
}

void MediaController::onMetadataChanged(const QMediaMetaData &metaData)
{
    emit metadataChanged(metaData);
}

void MediaController::onDurationChanged(qint64 duration)
{
    emit durationChanged(duration);
}

void MediaController::onPositionChanged(qint64 position)
{
    emit positionChanged(position);
}

void MediaController::onPlaybackStateChanged(QMediaPlayer::PlaybackState state)
{
    emit playbackStateChanged(state);
}

void MediaController::onHasVideoChanged(bool hasVideo)
{
    emit hasVideoChanged(hasVideo);
}
