#include "lyricmanager.h"
#include <QVBoxLayout>

LyricManager::LyricManager(QObject *parent)
    : QObject(parent),
      m_model(nullptr),
      m_view(nullptr),
      m_controller(nullptr)
{
}

LyricManager::~LyricManager()
{
    // 自动清理资源
}

void LyricManager::initialize(QMediaPlayer *player, QWidget *container)
{
    // 创建MVC组件
    m_model = new LyricModel(this);
    m_view = new LyricView(container);
    m_controller = new LyricController(this);
    
    // 连接组件
    m_controller->setModel(m_model);
    m_controller->setView(m_view);
    //m_controller->setMediaPlayer(player);
    
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

LyricView* LyricManager::getLyricView() const
{
    return m_view;
}

LyricController* LyricManager::getLyricController() const
{
    return m_controller;
}
