#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mymediaplayer.h"
#include <QPixmap>
#include <QFile>
#include <QImage>
#include <QFileInfo>
#include <QMessageBox>
#include <QFileDialog>
#include <QAudioOutput>
#include <QDir>
#include <QFileInfoList>
#include "qDebug"
#include <QDirIterator>
#include <QListWidgetItem>
#include <QString>
#include <QSlider>
#include <QtConcurrent>
#include <QVideoWidget>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_playButton_clicked();

    void on_stopButton_clicked();

    void on_volumeSlider_sliderMoved();

    void on_searchButton_clicked();

    void on_musiclist_itemDoubleClicked();

    void on_pushButton_clicked();

    void on_musiclist_itemClicked();

    void on_addplaylist_clicked();

    void on_playlist_currentRowChanged(int currentRow);

    void on_delplaylist_clicked();

    void on_autoplaycheckBox_checkStateChanged(const Qt::CheckState &arg1);

    void on_musicPostionSlider_sliderPressed();

    void on_opendir_clicked();

private:
    bool autonext=false;
    QString express;
    mymediaplayer *player;
    QLabel *pic;
    QVideoWidget *video;
    Ui::MainWindow *ui;
    QString path;
    QStringList extensions = {"mp3", "flac" , "wav" , "mkv" , "mp4"};
    int currentplaylistindex=0,maxplaylistindex=0;
    QList<QStringList> *playlists;
};
#endif // MAINWINDOW_H
