#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 创建并显示主窗口
    MainWindow w;
    w.show();
    
    return a.exec();
}
