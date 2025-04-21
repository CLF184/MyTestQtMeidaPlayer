QT       += core gui multimedia multimediawidgets concurrent sql
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
    manager/lyricmanager.cpp \
    manager/playlistmanager.cpp \
    manager/mediaplayermanager.cpp

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
    manager/lyricmanager.h \
    manager/playlistmanager.h \
    manager/mediaplayermanager.h

FORMS += \
    mainwindow.ui \
    views/mediaplayerview.ui \
    views/playlistview.ui \
    views/lyricview.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=
