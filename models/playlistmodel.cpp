#include "playlistmodel.h"

PlaylistModel::PlaylistModel(QObject *parent)
    : QObject(parent),
      m_selectedPlaylistId(0)
{
    m_db = new mysqlite(this);
    
    m_model = new QSqlTableModel(this);
    m_model->setTable("Playlist");
    
    connect(m_db, &mysqlite::playlistChanged, this, &PlaylistModel::playlistChanged);
    
    // 检查是否需要创建默认播放列表
    m_model->select();
    if(m_model->rowCount() == 0) {
        m_db->addPlaylist("默认播放列表");
        m_model->select();
        m_selectedPlaylistId = 1; // 默认选中第一个播放列表
    }
}

PlaylistModel::~PlaylistModel()
{
    // mysqlite和QSqlTableModel会由Qt的父子对象机制自动删除
}

QSqlTableModel* PlaylistModel::getModel()
{
    return m_model;
}

void PlaylistModel::addPlaylist(const QString &name)
{
    m_db->addPlaylist(name);
    m_model->select();
}

void PlaylistModel::removePlaylist(int playlistId)
{
    m_db->removePlaylist(playlistId);
    m_model->select();
}

int PlaylistModel::getSelectedPlaylistId() const
{
    return m_selectedPlaylistId;
}

void PlaylistModel::setSelectedPlaylistId(int id)
{
    if (m_selectedPlaylistId != id) {
        m_selectedPlaylistId = id;
        emit selectedPlaylistChanged(id);
    }
}
