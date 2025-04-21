#ifndef MEDIAMODEL_H
#define MEDIAMODEL_H

#include <QObject>
#include <QSqlTableModel>
#include <QStringList>
#include <QFuture>
#include <QMediaMetaData>
#include "../utils/mysqlite.h"
#include "../utils/mymediaplayer.h"
#include "../models/lyricmodel.h" // 添加此行

class MediaModel : public QObject
{
    Q_OBJECT
public:
    explicit MediaModel(QObject *parent = nullptr);
    ~MediaModel();

    QSqlTableModel* getModel();
    void scanDirectory(const QString &dir);
    void loadSongsForPlaylist(int playlistId);
    QString getSongPath(int songId) const;
    void setCurrentSongIndex(int index);
    int getCurrentSongIndex() const;
    
    // 播放器控制相关
    mymediaplayer* getPlayer();
    void setMedia(const QString &path);
    void play();
    void pause();
    void stop();
    void setVolume(int volume);
    void setPosition(int position);
    bool isAutoPlayEnabled() const;
    void setAutoPlayEnabled(bool enabled);
    void playNext();

signals:
    void mediaChanged();
    void currentSongChanged(int index);
    void metadataChanged(const QMediaMetaData &metaData);
    void durationChanged(qint64 duration);
    void positionChanged(qint64 position);
    void playbackStateChanged(QMediaPlayer::PlaybackState state);
    void hasVideoChanged(bool hasVideo);
    // 添加新信号，用于通知媒体文件路径变化
    void lyricChanged(const QString &path,const QMediaMetaData &metadata);

private slots:
    void handleMetadataChange();
    void handlePositionChange(qint64 position);
    void handleDurationChange(qint64 duration);
    void handlePlaybackStateChange(QMediaPlayer::PlaybackState state);
    void handleMediaStatusChange(QMediaPlayer::MediaStatus status);
    void handelLyricChanged();

private:
    mysqlite *m_db;
    QSqlTableModel *m_model;
    mymediaplayer *m_player;
    int m_currentPlaylistId;
    int m_currentSongIndex;
    bool m_autoPlayEnabled;
    QStringList m_supportedExtensions;
    QString m_currentSongPath;
    
    // 此方法可以删除，或保留但简化实现
    QString findLyricFile(const QString &mediaPath);
};

#endif // MEDIAMODEL_H
