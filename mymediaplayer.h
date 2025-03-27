#ifndef MYMEDIAPLAYER_H
#define MYMEDIAPLAYER_H

#include <QObject>
#include "QMediaPlayer"
#include "QAudioOutput"
#include "QImage"
#include "QMediaPlayer"
#include "QMediaMetaData"


class mymediaplayer :public QMediaPlayer
{
    Q_OBJECT
public:
    explicit mymediaplayer(QObject *parent = nullptr);
    QAudioOutput *audio;
    void setVolume(int volume);
    void setMedia(const QString &path);
    void setfilePosition(int position);

signals:
};

#endif // MYMEDIAPLAYER_H
