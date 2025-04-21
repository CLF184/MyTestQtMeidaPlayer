#ifndef MEDIAPLAYERMANAGER_H
#define MEDIAPLAYERMANAGER_H

#include <QObject>
#include <QWidget>
#include "../models/mediamodel.h"
#include "../controllers/mediacontroller.h"
#include "../views/mediaplayerview.h"

// 媒体播放器管理器类 - 负责协调媒体播放功能的整体流程
class MediaPlayerManager : public QObject
{
    Q_OBJECT
public:
    explicit MediaPlayerManager(QObject *parent = nullptr);
    ~MediaPlayerManager();
    
    // 初始化媒体播放系统
    void initialize(QWidget *container);
    
    // 获取媒体播放视图
    MediaPlayerView* getMediaPlayerView() const;
    
    // 获取媒体控制器
    MediaController* getMediaController() const;
    
    // 获取媒体模型
    MediaModel* getMediaModel() const;
    
private:
    MediaModel *m_model;
    MediaPlayerView *m_view;
    MediaController *m_controller;
};

#endif // MEDIAPLAYERMANAGER_H
