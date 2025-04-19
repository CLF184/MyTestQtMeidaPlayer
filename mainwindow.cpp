#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHBoxLayout>

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
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    
    // 添加视图到主布局
    mainLayout->addWidget(m_playlistView);
    mainLayout->addWidget(m_mediaPlayerView);
    
    // 设置布局比例
    mainLayout->setStretch(0, 1);  // 播放列表视图占30%
    mainLayout->setStretch(1, 2);  // 媒体播放器视图占70%
    
    // 设置中央部件
    setCentralWidget(centralWidget);
    
    // 设置最小窗口大小
    setMinimumSize(800, 500);
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
    
    // 设置视图的控制器
    m_playlistView->setController(m_playlistController);
    m_mediaPlayerView->setController(m_mediaController);
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


