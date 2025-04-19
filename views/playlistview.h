#ifndef PLAYLISTVIEW_H
#define PLAYLISTVIEW_H

#include <QWidget>
#include <QListView>
#include <QPushButton>
#include "../controllers/playlistcontroller.h"

namespace Ui {
class PlaylistView;
}

class PlaylistView : public QWidget
{
    Q_OBJECT

public:
    explicit PlaylistView(QWidget *parent = nullptr);
    ~PlaylistView();

    void setController(PlaylistController *controller);
    QListView* getPlaylistView();
    QListView* getSongListView();

signals:
    void playlistSelected(int playlistId);
    void songSelected(int songId);

private slots:
    void onAddPlaylistClicked();
    void onDeletePlaylistClicked();
    void onPlaylistClicked(const QModelIndex &index);
    void onSongListClicked(const QModelIndex &index);
    void onOpenDirClicked();
    void onSearchClicked();

private:
    Ui::PlaylistView *ui;
    PlaylistController *m_controller;
    QString m_currentDirectory;
};

#endif // PLAYLISTVIEW_H
