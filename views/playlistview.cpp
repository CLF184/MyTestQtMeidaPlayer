#include "playlistview.h"
#include "ui_playlistview.h"
#include <QFileDialog>
#include <QMessageBox>
#include  <./utils/urifix.h>

PlaylistView::PlaylistView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlaylistView),
    m_controller(nullptr)
{
    ui->setupUi(this);
    
    // 连接信号和槽
    connect(ui->addPlaylistButton, &QPushButton::clicked, this, &PlaylistView::onAddPlaylistClicked);
    connect(ui->deletePlaylistButton, &QPushButton::clicked, this, &PlaylistView::onDeletePlaylistClicked);
    connect(ui->openDirButton, &QPushButton::clicked, this, &PlaylistView::onOpenDirClicked);
    connect(ui->searchButton, &QPushButton::clicked, this, &PlaylistView::onSearchClicked);
    
    connect(ui->playlistView, &QListView::clicked, this, &PlaylistView::onPlaylistClicked);
    connect(ui->songListView, &QListView::clicked, this, &PlaylistView::onSongListClicked);
}

PlaylistView::~PlaylistView()
{
    delete ui;
}

void PlaylistView::setController(PlaylistController *controller)
{
    m_controller = controller;
    
    if (m_controller) {
        // 设置模型
        ui->playlistView->setModel(m_controller->getPlaylistModel());
        ui->playlistView->setModelColumn(1); // 播放列表名称所在的列
        
        ui->songListView->setModel(m_controller->getMediaModel());
        ui->songListView->setModelColumn(1); // 歌曲名称所在的列
    }
}

QListView* PlaylistView::getPlaylistView()
{
    return ui->playlistView;
}

QListView* PlaylistView::getSongListView()
{
    return ui->songListView;
}

void PlaylistView::onAddPlaylistClicked()
{
    if (m_controller) {
        m_controller->addNewPlaylist();
    }
}

void PlaylistView::onDeletePlaylistClicked()
{
    if (!m_controller) return;
    
    QModelIndex currentIndex = ui->playlistView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, tr("删除播放列表"), tr("请先选择一个播放列表"));
        return;
    }
    
    int playlistId = m_controller->getPlaylistIdAt(currentIndex.row());
    m_controller->removePlaylist(playlistId);
}

void PlaylistView::onPlaylistClicked(const QModelIndex &index)
{
    if (!m_controller || !index.isValid()) return;
    
    int playlistId = m_controller->getPlaylistIdAt(index.row());
    m_controller->selectPlaylist(playlistId);
    
    emit playlistSelected(playlistId);
}

void PlaylistView::onSongListClicked(const QModelIndex &index)
{
    if (!m_controller || !index.isValid()) return;
    
    int songId = m_controller->getSongIdAt(index.row());
    emit songSelected(songId);
    
    // 通知控制器播放选中的歌曲
    m_controller->playSong(index.row());
}

void PlaylistView::onOpenDirClicked()
{
    m_currentDirectory = QFileDialog::getExistingDirectory(
        this, 
        tr("选择音乐文件夹"),
        "", 
        QFileDialog::ShowDirsOnly
    );
    m_currentDirectory=URIFix::toPath(m_currentDirectory);
}

void PlaylistView::onSearchClicked()
{
    if (m_currentDirectory.isEmpty()) {
        QMessageBox::information(this, tr("搜索音乐"), tr("请先选择一个文件夹"));
        return;
    }
    
    QModelIndex currentIndex = ui->playlistView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, tr("搜索音乐"), tr("请先选择一个播放列表"));
        return;
    }
    
    // 显示搜索开始的消息
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("搜索中"));
    msgBox.setText(tr("正在搜索媒体文件，请稍候..."));
    msgBox.setStandardButtons(QMessageBox::NoButton);
    
    // 在不阻塞UI的情况下显示消息框
    msgBox.show();
    QCoreApplication::processEvents();
    
    if (m_controller) {
        m_controller->scanDirectory(m_currentDirectory);
    }
    
    // 关闭消息框并显示完成消息
    msgBox.close();
    QMessageBox::information(this, tr("搜索完成"), tr("媒体文件搜索完成！"));
}
