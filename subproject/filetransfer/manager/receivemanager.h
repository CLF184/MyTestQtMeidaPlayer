#ifndef RECEIVEMANAGER_H
#define RECEIVEMANAGER_H

#include <QObject>
#include "../model/receivemodel.h"
#include "../views/receiveview.h"
#include "../controller/receivecontroller.h"

class ReceiveManager : public QObject
{
    Q_OBJECT
public:
    explicit ReceiveManager(QObject *parent = nullptr);
    ~ReceiveManager();

    void init(QWidget *container);
    ReceiveView* getView() const;

private:
    ReceiveModel *m_model;
    ReceiveView *m_view;
    ReceiveController *m_controller;
};

#endif // RECEIVEMANAGER_H
