#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    // 设置窗口标题
    setWindowTitle(tr("音乐播放器"));
    
    // 初始化MVC组件
    setupModels();
    setupControllers();
    setupViews();
    setupConnections();
    
    // 创建主布局
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // 创建上部布局（播放列表和媒体播放器）
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(m_playlistView);
    topLayout->addWidget(m_mediaPlayerView);
    
    // 设置布局比例
    topLayout->setStretch(0, 1);  // 播放列表视图占30%
    topLayout->setStretch(1, 2);  // 媒体播放器视图占70%
    
    // 添加布局到主布局
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(m_lyricView); // 添加歌词视图到主布局底部
    
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

void MainWindow::setupModels()
{
    m_playlistModel = new PlaylistModel(this);
    m_mediaModel = new MediaModel(this);
}

void MainWindow::setupControllers()
{
    m_playlistController = new PlaylistController(m_playlistModel, m_mediaModel, this);
    m_mediaController = new MediaController(m_mediaModel, this);
}

void MainWindow::setupViews()
{
    m_playlistView = new PlaylistView(this);
    m_mediaPlayerView = new MediaPlayerView(this);
    m_lyricView = new LyricView(this);
    
    // 设置视图的控制器
    m_playlistView->setController(m_playlistController);
    m_mediaPlayerView->setController(m_mediaController);
    
    // 连接歌词信号
    connect(m_mediaController, &MediaController::lyricChanged,
            m_lyricView, &LyricView::updateLyric);
}

void MainWindow::setupConnections()
{
    // 当选择播放列表时，加载其中的歌曲
    connect(m_playlistView, &PlaylistView::playlistSelected, m_playlistController, &PlaylistController::selectPlaylist);
    
    // 当选择歌曲时，通知控制器播放歌曲
    connect(m_playlistView, &PlaylistView::songSelected, [this](int /*songId*/) {
        // 添加注释以说明参数未使用
        // 这里可以添加任何在歌曲选中时需要执行的额外逻辑
    });
}


