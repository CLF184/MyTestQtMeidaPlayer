#ifndef LYRICMANAGER_H
#define LYRICMANAGER_H

#include <QObject>
#include <QMediaPlayer>
#include <QWidget>
#include "../models/lyricmodel.h"
#include "../controllers/lyriccontroller.h"
#include "../views/lyricview.h"

// 歌词管理器类 - 负责协调歌词功能的整体流程
class LyricManager : public QObject
{
    Q_OBJECT
public:
    explicit LyricManager(QObject *parent = nullptr);
    ~LyricManager();
    
    // 初始化歌词系统
    void initialize(QMediaPlayer *player, QWidget *container);
    
    // 获取歌词视图
    LyricView* getLyricView() const;
    
    // 添加获取歌词控制器的方法
    LyricController* getLyricController() const;
    
private:
    LyricModel *m_model;
    LyricView *m_view;
    LyricController *m_controller;
};

#endif // LYRICMANAGER_H
