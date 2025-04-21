#include "mediaplayermanager.h"
#include <QVBoxLayout>

MediaPlayerManager::MediaPlayerManager(QObject *parent)
    : QObject(parent),
      m_model(nullptr),
      m_view(nullptr),
      m_controller(nullptr)
{
}

MediaPlayerManager::~MediaPlayerManager()
{
    // 自动清理资源
}

void MediaPlayerManager::initialize(QWidget *container)
{
    // 创建MVC组件
    m_model = new MediaModel(this);
    m_view = new MediaPlayerView(container);
    m_controller = new MediaController(m_model, this);
    
    // 连接组件
    m_view->setController(m_controller);
    
    // 将视图添加到容器
    if (container) {
        QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(container->layout());
        if (layout) {
            layout->addWidget(m_view);
        } else {
            QVBoxLayout *newLayout = new QVBoxLayout(container);
            newLayout->addWidget(m_view);
            container->setLayout(newLayout);
        }
    }
}

MediaPlayerView* MediaPlayerManager::getMediaPlayerView() const
{
    return m_view;
}

MediaController* MediaPlayerManager::getMediaController() const
{
    return m_controller;
}

MediaModel* MediaPlayerManager::getMediaModel() const
{
    return m_model;
}
