#include "mediamodel.h"
#include <QDirIterator>
#include <QFileInfo>
#include <QtConcurrent>
#include <QDebug>

MediaModel::MediaModel(QObject *parent)
    : QObject(parent),
      m_currentPlaylistId(0),
      m_currentSongIndex(-1),
      m_autoPlayEnabled(false)
{
    m_db = new mysqlite(this);
    
    m_model = new QSqlTableModel(this);
    m_model->setTable("Song");
    
    m_player = new mymediaplayer(this);
    m_player->setVolume(50);  // 默认音量50%
    
    // 支持的媒体文件格式
    m_supportedExtensions = {"mp3", "flac", "wav", "mkv", "mp4"};
    
    // 连接信号和槽
    connect(m_db, &mysqlite::songChanged, this, &MediaModel::mediaChanged);
    connect(m_player, &QMediaPlayer::metaDataChanged, this, &MediaModel::handleMetadataChange);
    connect(m_player, &QMediaPlayer::positionChanged, this, &MediaModel::handlePositionChange);
    connect(m_player, &QMediaPlayer::durationChanged, this, &MediaModel::handleDurationChange);
    connect(m_player, &QMediaPlayer::playbackStateChanged, this, &MediaModel::handlePlaybackStateChange);
    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, &MediaModel::handleMediaStatusChange);
}

MediaModel::~MediaModel()
{
    // 父子对象关系会自动处理内存释放
}

QSqlTableModel* MediaModel::getModel()
{
    return m_model;
}

void MediaModel::scanDirectory(const QString &dir)
{
    if (m_currentPlaylistId <= 0 || dir.isEmpty()) {
        return;
    }
    
    QStringList filepaths;
    QFuture<void> future = QtConcurrent::run([=, &filepaths]() {
        // 使用QDirIterator进行递归搜索
        // QDirIterator::Subdirectories 标志使迭代器递归进入子目录
        QDirIterator it(dir, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QString filePath = it.next();
            QFileInfo fileInfo(filePath);
            // 只检查文件（不是目录）并且文件扩展名在支持的列表中
            if (fileInfo.isFile() && m_supportedExtensions.contains(fileInfo.suffix(), Qt::CaseInsensitive)) {
                filepaths.append(fileInfo.filePath());
                qDebug() << "找到媒体文件:" << fileInfo.filePath();
            }
        }
    });
    
    future.waitForFinished();
    
    if (filepaths.isEmpty()) {
        qDebug() << "在目录" << dir << "及其子目录中没有找到支持的媒体文件";
    } else {
        qDebug() << "共找到" << filepaths.size() << "个媒体文件";
    }
    
    for (const QString &path : filepaths) {
        m_db->addSong(path, m_currentPlaylistId);
    }
    
    m_model->select();
}

void MediaModel::loadSongsForPlaylist(int playlistId)
{
    if (playlistId <= 0) return;
    
    m_currentPlaylistId = playlistId;
    
    m_model->setFilter(QString("playlist_id = %1").arg(playlistId));
    m_model->select();
    
    // 重置当前歌曲索引
    m_currentSongIndex = -1;
}

QString MediaModel::getSongPath(int songId) const
{
    // 通过songId获取歌曲路径
    QSqlQuery query;
    query.prepare("SELECT song_path FROM Song WHERE id = :id");
    query.bindValue(":id", songId);
    
    if (query.exec() && query.next()) {
        return query.value(0).toString();
    }
    
    return QString();
}

void MediaModel::setCurrentSongIndex(int index)
{
    if (m_currentSongIndex != index && index >= 0 && index < m_model->rowCount()) {
        m_currentSongIndex = index;
        
        // 获取歌曲ID
        int songId = m_model->data(m_model->index(index, 0)).toInt();
        
        // 获取歌曲路径
        QString path = getSongPath(songId);
        
        if (!path.isEmpty()) {
            setMedia(path);
            emit currentSongChanged(index);
        }
    }
}

int MediaModel::getCurrentSongIndex() const
{
    return m_currentSongIndex;
}

mymediaplayer* MediaModel::getPlayer()
{
    return m_player;
}

void MediaModel::setMedia(const QString &path)
{
    m_player->setMedia(path);
}

void MediaModel::play()
{
    m_player->play();
}

void MediaModel::pause()
{
    m_player->pause();
}

void MediaModel::stop()
{
    m_player->stop();
}

void MediaModel::setVolume(int volume)
{
    m_player->setVolume(volume);
}

void MediaModel::setPosition(int position)
{
    m_player->setfilePosition(position);
}

bool MediaModel::isAutoPlayEnabled() const
{
    return m_autoPlayEnabled;
}

void MediaModel::setAutoPlayEnabled(bool enabled)
{
    m_autoPlayEnabled = enabled;
}

void MediaModel::playNext()
{
    int nextIndex = m_currentSongIndex + 1;
    if (nextIndex < m_model->rowCount()) {
        setCurrentSongIndex(nextIndex);
        play();
    }
}

void MediaModel::handleMetadataChange()
{
    emit metadataChanged(m_player->metaData());
    emit hasVideoChanged(m_player->hasVideo());
}

void MediaModel::handlePositionChange(qint64 position)
{
    emit positionChanged(position);
    
    // 自动播放下一首
    if (m_autoPlayEnabled && m_player->duration() > 0 && 
        position >= m_player->duration() - 1000 && // 接近结束
        m_currentSongIndex < m_model->rowCount() - 1) {
        playNext();
    }
}

void MediaModel::handleDurationChange(qint64 duration)
{
    emit durationChanged(duration);
}

void MediaModel::handlePlaybackStateChange(QMediaPlayer::PlaybackState state)
{
    emit playbackStateChanged(state);
}

void MediaModel::handleMediaStatusChange(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia && m_autoPlayEnabled) {
        playNext();
    }
}
