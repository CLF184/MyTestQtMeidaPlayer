#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QObject>
#include <QSqlTableModel>
#include "../utils/mysqlite.h"

class PlaylistModel : public QObject
{
    Q_OBJECT
public:
    explicit PlaylistModel(QObject *parent = nullptr);
    ~PlaylistModel();

    QSqlTableModel* getModel();
    void addPlaylist(const QString &name);
    void removePlaylist(int playlistId);
    int getSelectedPlaylistId() const;
    void setSelectedPlaylistId(int id);

signals:
    void playlistChanged();
    void selectedPlaylistChanged(int id);

private:
    mysqlite *m_db;
    QSqlTableModel *m_model;
    int m_selectedPlaylistId;
};

#endif // PLAYLISTMODEL_H
