#include "mediaplayerview.h"
#include "ui_mediaplayerview.h"
#include <QTime>
#include <QVBoxLayout>

MediaPlayerView::MediaPlayerView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MediaPlayerView),
    m_controller(nullptr)
{
    ui->setupUi(this);
    
    // 创建覆盖艺术和视频部件
    m_coverArtLabel = new QLabel(this);
    m_coverArtLabel->setAlignment(Qt::AlignCenter);
    m_coverArtLabel->setScaledContents(true);
    
    m_videoWidget = new QVideoWidget(this);
    
    // 添加到堆叠部件中
    ui->mediaWidget->addWidget(m_coverArtLabel);
    ui->mediaWidget->addWidget(m_videoWidget);
    
    // 默认显示封面
    ui->mediaWidget->setCurrentWidget(m_coverArtLabel);
    
    // 连接信号和槽
    connect(ui->playButton, &QPushButton::clicked, this, &MediaPlayerView::onPlayButtonClicked);
    connect(ui->pauseButton, &QPushButton::clicked, this, &MediaPlayerView::onPauseButtonClicked);
    connect(ui->stopButton, &QPushButton::clicked, this, &MediaPlayerView::onStopButtonClicked);
    connect(ui->volumeSlider, &QSlider::valueChanged, this, &MediaPlayerView::onVolumeSliderMoved);
    connect(ui->positionSlider, &QSlider::sliderPressed, this, &MediaPlayerView::onPositionSliderPressed);
    connect(ui->positionSlider, &QSlider::valueChanged, this, &MediaPlayerView::onPositionSliderMoved);
    
    // 使用新的 checkStateChanged 信号替代已弃用的 stateChanged
    connect(ui->autoPlayCheckBox, &QCheckBox::checkStateChanged, this, &MediaPlayerView::onAutoPlayCheckBoxChanged);
}

MediaPlayerView::~MediaPlayerView()
{
    delete ui;
}

void MediaPlayerView::setController(MediaController *controller)
{
    m_controller = controller;
    
    if (m_controller) {
        // 设置视频输出
        m_controller->setVideoOutput(m_videoWidget);
        
        // 连接模型信号到视图槽
        connect(m_controller, &MediaController::metadataChanged, 
                this, &MediaPlayerView::updateMetadata);
        connect(m_controller, &MediaController::durationChanged, 
                this, &MediaPlayerView::updateDuration);
        connect(m_controller, &MediaController::positionChanged, 
                this, &MediaPlayerView::updatePosition);
        connect(m_controller, &MediaController::playbackStateChanged, 
                this, &MediaPlayerView::updatePlaybackState);
        connect(m_controller, &MediaController::hasVideoChanged, 
                this, &MediaPlayerView::updateVideoVisibility);
    }
}

QVideoWidget* MediaPlayerView::getVideoWidget()
{
    return m_videoWidget;
}

void MediaPlayerView::onPlayButtonClicked()
{
    if (m_controller) {
        m_controller->play();
    }
}

void MediaPlayerView::onPauseButtonClicked()
{
    if (m_controller) {
        m_controller->pause();
    }
}

void MediaPlayerView::onStopButtonClicked()
{
    if (m_controller) {
        m_controller->stop();
    }
}

void MediaPlayerView::onVolumeSliderMoved()
{
    if (m_controller) {
        m_controller->setVolume(ui->volumeSlider->value());
    }
}

void MediaPlayerView::onPositionSliderPressed()
{
    if (m_controller) {
        m_controller->setPosition(ui->positionSlider->value());
    }
}

void MediaPlayerView::onPositionSliderMoved(int position)
{
    if (m_controller) {
        // 只有当用户拖动滑块时才设置位置
        if (ui->positionSlider->isSliderDown()) {
            m_controller->setPosition(position);
        }
    }
}

void MediaPlayerView::onAutoPlayCheckBoxChanged(int state)
{
    if (m_controller) {
        m_controller->setAutoPlayEnabled(state == Qt::Checked);
    }
}

void MediaPlayerView::updateMetadata(const QMediaMetaData &metaData)
{
    // 更新元数据标签
    ui->titleLabel->setText(tr("标题: %1").arg(metaData.value(QMediaMetaData::Title).toString()));
    ui->artistLabel->setText(tr("艺术家: %1").arg(metaData.value(QMediaMetaData::ContributingArtist).toString()));
    ui->albumLabel->setText(tr("专辑: %1").arg(metaData.value(QMediaMetaData::AlbumTitle).toString()));
    ui->yearLabel->setText(tr("年份: %1").arg(metaData.value(QMediaMetaData::Date).toString()));
    ui->bitrateLabel->setText(tr("比特率: %1").arg(metaData.value(QMediaMetaData::AudioBitRate).toString()));
    
    // 更新专辑封面
    QImage coverArt = metaData.value(QMediaMetaData::ThumbnailImage).value<QImage>();
    if (!coverArt.isNull()) {
        m_coverArtLabel->setPixmap(QPixmap::fromImage(coverArt));
    } else {
        m_coverArtLabel->clear();
    }
}

void MediaPlayerView::updateDuration(qint64 duration)
{
    ui->positionSlider->setMaximum(duration / 1000);
    formatTimeLabel(ui->totalTimeLabel, duration);
}

void MediaPlayerView::updatePosition(qint64 position)
{
    // 只有当滑块没被用户拖动时才更新
    if (!ui->positionSlider->isSliderDown()) {
        ui->positionSlider->setValue(position / 1000);
    }
    
    formatTimeLabel(ui->currentTimeLabel, position);
}

void MediaPlayerView::updatePlaybackState(QMediaPlayer::PlaybackState state)
{
    switch (state) {
    case QMediaPlayer::PlayingState:
        ui->playButton->setEnabled(false);
        ui->pauseButton->setEnabled(true);
        ui->stopButton->setEnabled(true);
        break;
    case QMediaPlayer::PausedState:
        ui->playButton->setEnabled(true);
        ui->pauseButton->setEnabled(false);
        ui->stopButton->setEnabled(true);
        break;
    case QMediaPlayer::StoppedState:
        ui->playButton->setEnabled(true);
        ui->pauseButton->setEnabled(false);
        ui->stopButton->setEnabled(false);
        break;
    }
}

void MediaPlayerView::updateVideoVisibility(bool hasVideo)
{
    // 修复类型转换问题
    if (hasVideo) {
        ui->mediaWidget->setCurrentWidget(m_videoWidget);
    } else {
        ui->mediaWidget->setCurrentWidget(m_coverArtLabel);
    }
}

void MediaPlayerView::formatTimeLabel(QLabel *label, qint64 timeInMs)
{
    int seconds = (timeInMs / 1000) % 60;
    int minutes = (timeInMs / 60000) % 60;
    int hours = (timeInMs / 3600000);
    
    QTime time(hours, minutes, seconds);
    QString format = hours > 0 ? "h:mm:ss" : "mm:ss";
    label->setText(time.toString(format));
}
