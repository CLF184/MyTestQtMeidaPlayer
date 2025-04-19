#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "models/mediamodel.h"
#include "models/playlistmodel.h"
#include "controllers/mediacontroller.h"
#include "controllers/playlistcontroller.h"
#include "views/mediaplayerview.h"
#include "views/playlistview.h"
#include "views/lyricview.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    
    // 模型
    MediaModel *m_mediaModel;
    PlaylistModel *m_playlistModel;
    
    // 控制器
    MediaController *m_mediaController;
    PlaylistController *m_playlistController;
    
    // 视图
    MediaPlayerView *m_mediaPlayerView;
    PlaylistView *m_playlistView;
    LyricView *m_lyricView;
    
    void setupModels();
    void setupControllers();
    void setupViews();
    void setupConnections();
};
#endif // MAINWINDOW_H
