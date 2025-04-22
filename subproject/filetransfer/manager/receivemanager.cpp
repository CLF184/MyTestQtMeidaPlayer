#include "receivemanager.h"
#include <QVBoxLayout>

ReceiveManager::ReceiveManager(QObject *parent)
    : QObject(parent),
      m_model(nullptr),
      m_view(nullptr),
      m_controller(nullptr)
{
}

ReceiveManager::~ReceiveManager()
{
    // 组件会被自动释放，因为它们有父对象
}

void ReceiveManager::init(QWidget *container)
{
    // 创建模型
    m_model = new ReceiveModel(this);
    
    // 创建视图
    m_view = new ReceiveView();
    
    // 创建控制器
    m_controller = new ReceiveController(m_model, m_view, this);
    
    // 如果提供了容器，将视图添加到容器中
    if (container) {
        QVBoxLayout *layout = new QVBoxLayout(container);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(m_view);
        container->setLayout(layout);
    }
}

ReceiveView* ReceiveManager::getView() const
{
    return m_view;
}
