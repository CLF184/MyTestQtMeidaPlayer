#ifndef MYSQLITE_H
#define MYSQLITE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlTableModel>
#include <QDebug>
#include <QFileInfo>

class mysqlite : public QObject
{
    Q_OBJECT
public:
    mysqlite(QObject *parent = nullptr);
    ~mysqlite();
    
    // 播放列表相关操作
    void addPlaylist(QString playlistName);
    void removePlaylist(int playlistId);
    
    // 歌曲相关操作
    void addSong(QString songPath, int playlistId);
    void removeSong(int songId);
    QString getSongPath(int playlistId, int songId);

signals:
    void songChanged();
    void playlistChanged();

private:
    QSqlDatabase db;
    void initializeDatabase();
};

#endif // MYSQLITE_H
