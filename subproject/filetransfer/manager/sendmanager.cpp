#include "sendmanager.h"
#include <QVBoxLayout>

SendManager::SendManager(QObject *parent)
    : QObject(parent),
      m_model(nullptr),
      m_view(nullptr),
      m_controller(nullptr)
{
}

SendManager::~SendManager()
{
    // 组件会被自动释放，因为它们有父对象
}

void SendManager::init(QWidget *container)
{
    // 创建模型
    m_model = new SendModel(this);
    
    // 创建视图
    m_view = new SendView();
    
    // 创建控制器
    m_controller = new SendController(m_model, m_view, this);
    
    // 如果提供了容器，将视图添加到容器中
    if (container) {
        QVBoxLayout *layout = new QVBoxLayout(container);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(m_view);
        container->setLayout(layout);
    }
}

SendView* SendManager::getView() const
{
    return m_view;
}
