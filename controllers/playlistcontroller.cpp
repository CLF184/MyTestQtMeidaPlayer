#include "playlistcontroller.h"

// 静态变量初始化
int PlaylistController::s_playlistCounter = 1;

PlaylistController::PlaylistController(PlaylistModel *playlistModel, MediaModel *mediaModel, QObject *parent)
    : QObject(parent),
      m_playlistModel(playlistModel),
      m_mediaModel(mediaModel)
{
    // 连接播放列表选择变更信号
    connect(m_playlistModel, &PlaylistModel::selectedPlaylistChanged, this, &PlaylistController::selectPlaylist);
}

QSqlTableModel* PlaylistController::getPlaylistModel()
{
    return m_playlistModel ? m_playlistModel->getModel() : nullptr;
}

QSqlTableModel* PlaylistController::getMediaModel()
{
    return m_mediaModel ? m_mediaModel->getModel() : nullptr;
}

void PlaylistController::addNewPlaylist()
{
    if (m_playlistModel) {
        QString playlistName = QString("播放列表 %1").arg(++s_playlistCounter);
        m_playlistModel->addPlaylist(playlistName);
        
        // 获取新增播放列表的ID
        QSqlTableModel *model = m_playlistModel->getModel();
        model->select();
        int lastRow = model->rowCount() - 1;
        if (lastRow >= 0) {
            int playlistId = model->data(model->index(lastRow, 0)).toInt();
            emit playlistAdded(playlistId);
        }
    }
}

void PlaylistController::removePlaylist(int playlistId)
{
    if (m_playlistModel) {
        m_playlistModel->removePlaylist(playlistId);
        emit playlistRemoved(playlistId);
    }
}

void PlaylistController::selectPlaylist(int playlistId)
{
    if (m_playlistModel && m_mediaModel) {
        m_playlistModel->setSelectedPlaylistId(playlistId);
        m_mediaModel->loadSongsForPlaylist(playlistId);
        emit playlistSelected(playlistId);
        emit songsLoaded();
    }
}

int PlaylistController::getPlaylistIdAt(int row)
{
    QSqlTableModel *model = m_playlistModel->getModel();
    return model->data(model->index(row, 0)).toInt();
}

void PlaylistController::scanDirectory(const QString &dir)
{
    if (m_mediaModel) {
        // 确保当前选定了播放列表
        int playlistId = m_playlistModel->getSelectedPlaylistId();
        if (playlistId <= 0) {
            qDebug() << "没有选择播放列表，无法扫描目录";
            return;
        }
        
        qDebug() << "开始扫描目录:" << dir << "为播放列表ID:" << playlistId;
        m_mediaModel->scanDirectory(dir);
    }
}

int PlaylistController::getSongIdAt(int row)
{
    QSqlTableModel *model = m_mediaModel->getModel();
    return model->data(model->index(row, 0)).toInt();
}

void PlaylistController::playSong(int row)
{
    if (m_mediaModel) {
        m_mediaModel->setCurrentSongIndex(row);
        m_mediaModel->play();
    }
}
