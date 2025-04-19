#ifndef PLAYLISTCONTROLLER_H
#define PLAYLISTCONTROLLER_H

#include <QObject>
#include <QSqlTableModel>
#include "../models/playlistmodel.h"
#include "../models/mediamodel.h"

class PlaylistController : public QObject
{
    Q_OBJECT
public:
    explicit PlaylistController(PlaylistModel *playlistModel, MediaModel *mediaModel, QObject *parent = nullptr);
    
    // 返回模型用于视图展示
    QSqlTableModel* getPlaylistModel();
    QSqlTableModel* getMediaModel();
    
    // 播放列表相关操作
    void addNewPlaylist();
    void removePlaylist(int playlistId);
    void selectPlaylist(int playlistId);
    int getPlaylistIdAt(int row);
    
    // 媒体相关操作
    void scanDirectory(const QString &dir);
    int getSongIdAt(int row);
    void playSong(int row);

signals:
    // 传递给视图的信号
    void playlistAdded(int playlistId);
    void playlistRemoved(int playlistId);
    void playlistSelected(int playlistId);
    void songsLoaded();

private:
    PlaylistModel *m_playlistModel;
    MediaModel *m_mediaModel;
    static int s_playlistCounter;
};

#endif // PLAYLISTCONTROLLER_H
