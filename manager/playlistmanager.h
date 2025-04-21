#ifndef PLAYLISTMANAGER_H
#define PLAYLISTMANAGER_H

#include <QObject>
#include <QWidget>
#include "../models/playlistmodel.h"
#include "../controllers/playlistcontroller.h"
#include "../views/playlistview.h"
#include "../models/mediamodel.h"

// 播放列表管理器类 - 负责协调播放列表功能的整体流程
class PlaylistManager : public QObject
{
    Q_OBJECT
public:
    explicit PlaylistManager(QObject *parent = nullptr);
    ~PlaylistManager();
    
    // 初始化播放列表系统
    void initialize(MediaModel *mediaModel, QWidget *container);
    
    // 获取播放列表视图
    PlaylistView* getPlaylistView() const;
    
    // 获取播放列表控制器
    PlaylistController* getPlaylistController() const;
    
    // 获取播放列表模型
    PlaylistModel* getPlaylistModel() const;
    
private:
    PlaylistModel *m_model;
    PlaylistView *m_view;
    PlaylistController *m_controller;
};

#endif // PLAYLISTMANAGER_H
