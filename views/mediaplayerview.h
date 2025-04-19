#ifndef MEDIAPLAYERVIEW_H
#define MEDIAPLAYERVIEW_H

#include <QWidget>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QCheckBox>
#include <QStackedWidget>
#include <QVideoWidget>
#include "../controllers/mediacontroller.h"

namespace Ui {
class MediaPlayerView;
}

class MediaPlayerView : public QWidget
{
    Q_OBJECT

public:
    explicit MediaPlayerView(QWidget *parent = nullptr);
    ~MediaPlayerView();

    void setController(MediaController *controller);
    QVideoWidget* getVideoWidget();

private slots:
    void onPlayButtonClicked();
    void onPauseButtonClicked();
    void onStopButtonClicked();
    void onVolumeSliderMoved();
    void onPositionSliderPressed();
    void onPositionSliderMoved(int position);
    void onAutoPlayCheckBoxChanged(int state);
    
    // 响应模型变化的槽
    void updateMetadata(const QMediaMetaData &metaData);
    void updateDuration(qint64 duration);
    void updatePosition(qint64 position);
    void updatePlaybackState(QMediaPlayer::PlaybackState state);
    void updateVideoVisibility(bool hasVideo);

private:
    Ui::MediaPlayerView *ui;
    MediaController *m_controller;
    QLabel *m_coverArtLabel;
    QVideoWidget *m_videoWidget;
    
    void formatTimeLabel(QLabel *label, qint64 timeInMs);
};

#endif // MEDIAPLAYERVIEW_H
