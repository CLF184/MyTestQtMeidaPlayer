#include "mymediaplayer.h"
#include "qDebug"

mymediaplayer::mymediaplayer(QObject *parent)
    :QMediaPlayer{parent}
{
    audio = new QAudioOutput();
    this->setAudioOutput(audio);
    audio->setVolume(0.5);
}

void mymediaplayer::setMedia(const QString &path){
    qDebug()<<path;
    setSource(QUrl::fromLocalFile(path));
}

void mymediaplayer::setVolume(int volume){
    audio->setVolume((double)volume/100);
    setAudioOutput(audio);
}


void mymediaplayer::setfilePosition(int postiton){
    setPosition(postiton*1000);
}

