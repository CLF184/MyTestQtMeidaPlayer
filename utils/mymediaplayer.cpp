#include "mymediaplayer.h"
#include <QDebug>
#include <QUrl>

mymediaplayer::mymediaplayer(QObject *parent)
    : QMediaPlayer(parent)
{
    audio = new QAudioOutput(this);
    setAudioOutput(audio);
    audio->setVolume(0.5); // 默认音量50%
}

mymediaplayer::~mymediaplayer()
{
    // QAudioOutput会通过父子关系自动释放
}

void mymediaplayer::setVolume(int volume)
{
    // 将0-100的音量值转换为0.0-1.0
    qreal normalizedVolume = volume / 100.0;
    audio->setVolume(normalizedVolume);
}

void mymediaplayer::setMedia(const QString &path)
{
    qDebug() << "设置媒体源:" << path;
    setSource(QUrl::fromLocalFile(path));
}

void mymediaplayer::setfilePosition(int position)
{
    // 将秒转换为毫秒
    setPosition(position * 1000);
}

QAudioOutput* mymediaplayer::getAudioOutput() const
{
    return audio;
}
