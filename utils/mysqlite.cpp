#include "mysqlite.h"

mysqlite::mysqlite(QObject *parent) : QObject(parent)
{
    initializeDatabase();
}

mysqlite::~mysqlite()
{
    if (db.isOpen()) {
        db.close();
    }
}

void mysqlite::initializeDatabase()
{
    // 建立数据库连接
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("music.db");
    
    if (!db.open()) {
        QSqlError error = db.lastError();
        qDebug() << "数据库打开失败:" << error.text();
        return;
    }
    
    QSqlQuery query;
    
    // 1. 创建播放列表表
    QString createPlaylistTable =
        "CREATE TABLE IF NOT EXISTS Playlist ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "playlist_name TEXT)";
    
    if (!query.exec(createPlaylistTable)) {
        qDebug() << "创建 Playlist 表失败:" << query.lastError().text();
    } else {
        qDebug() << "Playlist 表创建成功或已存在";
    }
    
    // 2. 创建歌曲表
    QString createSongTable =
        "CREATE TABLE IF NOT EXISTS Song ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "song_title TEXT, "
        "song_path TEXT, "
        "playlist_id INTEGER, "
        "FOREIGN KEY(playlist_id) REFERENCES Playlist(id) ON DELETE CASCADE)";
    
    if (!query.exec(createSongTable)) {
        qDebug() << "创建 Song 表失败:" << query.lastError().text();
    } else {
        qDebug() << "Song 表创建成功或已存在";
    }
    
    // 启用外键约束
    query.exec("PRAGMA foreign_keys = ON");
}

void mysqlite::addPlaylist(QString playlistName)
{
    QSqlQuery query;
    query.prepare("INSERT INTO Playlist (playlist_name) VALUES (:name)");
    query.bindValue(":name", playlistName);
    
    if (!query.exec()) {
        qDebug() << "添加播放列表失败:" << query.lastError().text();
    } else {
        emit playlistChanged();
    }
}

void mysqlite::removePlaylist(int playlistId)
{
    QSqlQuery query;
    
    // 首先删除该播放列表下的所有歌曲
    query.prepare("DELETE FROM Song WHERE playlist_id = :id");
    query.bindValue(":id", playlistId);
    
    if (!query.exec()) {
        qDebug() << "删除播放列表下歌曲失败:" << query.lastError().text();
        return;
    }
    
    // 然后删除播放列表
    query.prepare("DELETE FROM Playlist WHERE id = :id");
    query.bindValue(":id", playlistId);
    
    if (!query.exec()) {
        qDebug() << "删除播放列表失败:" << query.lastError().text();
    } else {
        emit playlistChanged();
    }
}

void mysqlite::addSong(QString songPath, int playlistId)
{
    QSqlQuery query;
    
    // 从路径中提取文件名作为歌曲标题
    QFileInfo fileInfo(songPath);
    QString songTitle = fileInfo.fileName();
    
    query.prepare("INSERT INTO Song (song_title, song_path, playlist_id) "
                  "VALUES (:title, :path, :playlist_id)");
    query.bindValue(":title", songTitle);
    query.bindValue(":path", songPath);
    query.bindValue(":playlist_id", playlistId);
    
    if (!query.exec()) {
        qDebug() << "添加歌曲失败:" << query.lastError().text();
    } else {
        emit songChanged();
    }
}

void mysqlite::removeSong(int songId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM Song WHERE id = :id");
    query.bindValue(":id", songId);
    
    if (!query.exec()) {
        qDebug() << "删除歌曲失败:" << query.lastError().text();
    } else {
        emit songChanged();
    }
}

QString mysqlite::getSongPath(int playlistId, int songId)
{
    QSqlQuery query;
    query.prepare("SELECT song_path FROM Song WHERE playlist_id = :playlist_id AND id = :song_id");
    query.bindValue(":playlist_id", playlistId);
    query.bindValue(":song_id", songId);
    
    if (query.exec() && query.next()) {
        return query.value(0).toString();
    } else {
        qDebug() << "获取歌曲路径失败:" << query.lastError().text();
        return QString();
    }
}
