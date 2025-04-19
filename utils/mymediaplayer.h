#ifndef MYMEDIAPLAYER_H
#define MYMEDIAPLAYER_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QImage>
#include <QMediaMetaData>

class mymediaplayer : public QMediaPlayer
{
    Q_OBJECT
public:
    explicit mymediaplayer(QObject *parent = nullptr);
    ~mymediaplayer();
    
    void setVolume(int volume);
    void setMedia(const QString &path);
    void setfilePosition(int position);
    QAudioOutput* getAudioOutput() const;

private:
    QAudioOutput *audio;
};

#endif // MYMEDIAPLAYER_H
