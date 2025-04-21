#include "playlistmanager.h"
#include <QVBoxLayout>

PlaylistManager::PlaylistManager(QObject *parent)
    : QObject(parent),
      m_model(nullptr),
      m_view(nullptr),
      m_controller(nullptr)
{
}

PlaylistManager::~PlaylistManager()
{
    // 自动清理资源
}

void PlaylistManager::initialize(MediaModel *mediaModel, QWidget *container)
{
    // 创建MVC组件
    m_model = new PlaylistModel(this);
    m_view = new PlaylistView(container);
    m_controller = new PlaylistController(m_model, mediaModel, this);
    
    // 连接组件
    m_view->setController(m_controller);
    
    // 添加内部信号连接
    connect(m_view, &PlaylistView::playlistSelected, 
            m_controller, &PlaylistController::selectPlaylist);
    
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

PlaylistView* PlaylistManager::getPlaylistView() const
{
    return m_view;
}

PlaylistController* PlaylistManager::getPlaylistController() const
{
    return m_controller;
}

PlaylistModel* PlaylistManager::getPlaylistModel() const
{
    return m_model;
}
