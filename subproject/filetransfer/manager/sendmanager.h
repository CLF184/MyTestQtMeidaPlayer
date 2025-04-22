#ifndef SENDMANAGER_H
#define SENDMANAGER_H

#include <QObject>
#include "../model/sendmodel.h"
#include "../views/sendview.h"
#include "../controller/sendcontroller.h"

class SendManager : public QObject
{
    Q_OBJECT
public:
    explicit SendManager(QObject *parent = nullptr);
    ~SendManager();

    void init(QWidget *container);
    SendView* getView() const;

private:
    SendModel *m_model;
    SendView *m_view;
    SendController *m_controller;
};

#endif // SENDMANAGER_H
