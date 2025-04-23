#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "manager/mediaplayermanager.h"
#include "manager/playlistmanager.h"
#include "manager/lyricmanager.h"
#include "subproject/filetransfer/filetransfer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onFileTransferActionTriggered();

private:
    Ui::MainWindow *ui;
    
    // 管理器
    MediaPlayerManager *m_mediaPlayerManager;
    PlaylistManager *m_playlistManager;
    LyricManager *m_lyricManager;

    void setupManagers();
    void setupConnections();
    void setup();
};
#endif // MAINWINDOW_H
