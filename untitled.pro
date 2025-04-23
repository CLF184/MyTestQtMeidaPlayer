QT       += core gui multimedia multimediawidgets concurrent sql core-private
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    models/mediamodel.cpp \
    models/playlistmodel.cpp \
    models/lyricmodel.cpp \
    controllers/mediacontroller.cpp \
    controllers/playlistcontroller.cpp \
    controllers/lyriccontroller.cpp \
    views/mediaplayerview.cpp \
    views/playlistview.cpp \
    views/lyricview.cpp \
    utils/mymediaplayer.cpp \
    utils/mysqlite.cpp \
    utils/lyricparser.cpp \
    utils/urifix.cpp \
    manager/lyricmanager.cpp \
    manager/playlistmanager.cpp \
    manager/mediaplayermanager.cpp \
    subproject/filetransfer/filetransfer.cpp \
    subproject/filetransfer/model/sendmodel.cpp \
    subproject/filetransfer/controller/sendcontroller.cpp \
    subproject/filetransfer/views/sendview.cpp \
    subproject/filetransfer/manager/sendmanager.cpp \
    subproject/filetransfer/model/receivemodel.cpp \
    subproject/filetransfer/controller/receivecontroller.cpp \
    subproject/filetransfer/views/receiveview.cpp \
    subproject/filetransfer/manager/receivemanager.cpp \

HEADERS += \
    mainwindow.h \
    models/mediamodel.h \
    models/playlistmodel.h \
    models/lyricmodel.h \
    controllers/mediacontroller.h \
    controllers/playlistcontroller.h \
    controllers/lyriccontroller.h \
    views/mediaplayerview.h \
    views/playlistview.h \
    views/lyricview.h \
    utils/mymediaplayer.h \
    utils/mysqlite.h \
    utils/lyricparser.h \
    utils/urifix.h \
    manager/lyricmanager.h \
    manager/playlistmanager.h \
    manager/mediaplayermanager.h\
    subproject/filetransfer/filetransfer.h \
    subproject/filetransfer/model/sendmodel.h \
    subproject/filetransfer/controller/sendcontroller.h \
    subproject/filetransfer/views/sendview.h \
    subproject/filetransfer/manager/sendmanager.h \
    subproject/filetransfer/model/receivemodel.h \
    subproject/filetransfer/controller/receivecontroller.h \
    subproject/filetransfer/views/receiveview.h \
    subproject/filetransfer/manager/receivemanager.h \
    subproject/filetransfer/utils/urifix.h
    

FORMS += \
    mainwindow.ui \
    views/mediaplayerview.ui \
    views/playlistview.ui \
    views/lyricview.ui \
    subproject/filetransfer/filetransfer.ui \
    subproject/filetransfer/views/ReceiveView.ui \
    subproject/filetransfer/views/SendView.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=
