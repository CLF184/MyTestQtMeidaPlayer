#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_mediaPlayerManager(new MediaPlayerManager(this))
    , m_playlistManager(new PlaylistManager(this))
    , m_lyricManager(new LyricManager(this))
{
    ui->setupUi(this);
    
    // 设置窗口标题
    setWindowTitle(tr("音乐播放器"));
    
    // 初始化managers和连接
    setupManagers();
    setupConnections();
    
    // 创建主布局
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // 创建上部布局（播放列表和媒体播放器）
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(m_playlistManager->getPlaylistView());
    topLayout->addWidget(m_mediaPlayerManager->getMediaPlayerView());
    
    // 设置布局比例
    topLayout->setStretch(0, 1);  // 播放列表视图占30%
    topLayout->setStretch(1, 2);  // 媒体播放器视图占70%
    
    // 添加布局到主布局
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(m_lyricManager->getLyricView()); 
    
    // 设置布局比例
    mainLayout->setStretch(0, 3); // 上部区域占75%
    mainLayout->setStretch(1, 1); // 歌词区域占25%
    
    // 设置中央部件
    setCentralWidget(centralWidget);
    
    // 设置最小窗口大小
    setMinimumSize(800, 750);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupManagers()
{
    // 初始化媒体播放器系统
    m_mediaPlayerManager->initialize(nullptr);
    
    // 初始化播放列表系统，传入媒体模型
    m_playlistManager->initialize(m_mediaPlayerManager->getMediaModel(), nullptr);
    
    // 初始化歌词系统
    m_lyricManager->initialize(m_mediaPlayerManager->getMediaModel()->getPlayer(), nullptr);
}

void MainWindow::setupConnections()
{
    // 原有的连接
    connect(m_mediaPlayerManager->getMediaController(), &MediaController::lyricChanged,
            m_lyricManager->getLyricView(), &LyricView::updateLyric);
            
    // 添加从MediaPlayer到LyricController的跨模块连接
    QMediaPlayer *player = m_mediaPlayerManager->getMediaModel()->getPlayer();
    LyricController *lyricController = m_lyricManager->getLyricController();
    
    // 播放位置变化时更新歌词
    connect(player, &QMediaPlayer::positionChanged, 
            lyricController, &LyricController::onPositionChanged);
    
    // 保留这个连接：媒体路径变化信号连接到歌词控制器
    connect(m_mediaPlayerManager->getMediaModel(), &MediaModel::lyricChanged,
            lyricController, &LyricController::onLyricChanged);
}


