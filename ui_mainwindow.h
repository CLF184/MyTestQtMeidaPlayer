/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *playButton;
    QPushButton *pushButton;
    QPushButton *stopButton;
    QSlider *volumeSlider;
    QSlider *musicPostionSlider;
    QWidget *layoutWidget1;
    QHBoxLayout *horizontalLayout_2;
    QLabel *timeLabel;
    QLabel *musicLength;
    QWidget *layoutWidget2;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *delplaylist;
    QPushButton *addplaylist;
    QCheckBox *autoplaycheckBox;
    QWidget *layoutWidget3;
    QVBoxLayout *verticalLayout_2;
    QLabel *Title;
    QLabel *AlbumTitle;
    QLabel *ContributingArtist;
    QLabel *TrackNumber;
    QLabel *Date;
    QLabel *AudioBitRate;
    QWidget *layoutWidget4;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *opendir;
    QPushButton *searchButton;
    QStackedWidget *stackedWidget;
    QListView *playlist;
    QListView *songlist;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(750, 530);
        MainWindow->setMinimumSize(QSize(750, 200));
        MainWindow->setMaximumSize(QSize(750, 700));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        layoutWidget = new QWidget(centralwidget);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(224, 440, 261, 45));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        playButton = new QPushButton(layoutWidget);
        playButton->setObjectName("playButton");

        horizontalLayout->addWidget(playButton);

        pushButton = new QPushButton(layoutWidget);
        pushButton->setObjectName("pushButton");

        horizontalLayout->addWidget(pushButton);

        stopButton = new QPushButton(layoutWidget);
        stopButton->setObjectName("stopButton");

        horizontalLayout->addWidget(stopButton);


        verticalLayout->addLayout(horizontalLayout);

        volumeSlider = new QSlider(centralwidget);
        volumeSlider->setObjectName("volumeSlider");
        volumeSlider->setGeometry(QRect(720, 30, 16, 160));
        volumeSlider->setOrientation(Qt::Orientation::Vertical);
        musicPostionSlider = new QSlider(centralwidget);
        musicPostionSlider->setObjectName("musicPostionSlider");
        musicPostionSlider->setGeometry(QRect(190, 400, 281, 21));
        musicPostionSlider->setOrientation(Qt::Orientation::Horizontal);
        layoutWidget1 = new QWidget(centralwidget);
        layoutWidget1->setObjectName("layoutWidget1");
        layoutWidget1->setGeometry(QRect(490, 400, 86, 21));
        horizontalLayout_2 = new QHBoxLayout(layoutWidget1);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        timeLabel = new QLabel(layoutWidget1);
        timeLabel->setObjectName("timeLabel");

        horizontalLayout_2->addWidget(timeLabel);

        musicLength = new QLabel(layoutWidget1);
        musicLength->setObjectName("musicLength");

        horizontalLayout_2->addWidget(musicLength);

        layoutWidget2 = new QWidget(centralwidget);
        layoutWidget2->setObjectName("layoutWidget2");
        layoutWidget2->setGeometry(QRect(10, 180, 158, 26));
        horizontalLayout_3 = new QHBoxLayout(layoutWidget2);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        delplaylist = new QPushButton(layoutWidget2);
        delplaylist->setObjectName("delplaylist");

        horizontalLayout_3->addWidget(delplaylist);

        addplaylist = new QPushButton(layoutWidget2);
        addplaylist->setObjectName("addplaylist");

        horizontalLayout_3->addWidget(addplaylist);

        autoplaycheckBox = new QCheckBox(centralwidget);
        autoplaycheckBox->setObjectName("autoplaycheckBox");
        autoplaycheckBox->setGeometry(QRect(490, 460, 71, 16));
        layoutWidget3 = new QWidget(centralwidget);
        layoutWidget3->setObjectName("layoutWidget3");
        layoutWidget3->setGeometry(QRect(570, 20, 141, 171));
        verticalLayout_2 = new QVBoxLayout(layoutWidget3);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        Title = new QLabel(layoutWidget3);
        Title->setObjectName("Title");

        verticalLayout_2->addWidget(Title);

        AlbumTitle = new QLabel(layoutWidget3);
        AlbumTitle->setObjectName("AlbumTitle");

        verticalLayout_2->addWidget(AlbumTitle);

        ContributingArtist = new QLabel(layoutWidget3);
        ContributingArtist->setObjectName("ContributingArtist");

        verticalLayout_2->addWidget(ContributingArtist);

        TrackNumber = new QLabel(layoutWidget3);
        TrackNumber->setObjectName("TrackNumber");

        verticalLayout_2->addWidget(TrackNumber);

        Date = new QLabel(layoutWidget3);
        Date->setObjectName("Date");

        verticalLayout_2->addWidget(Date);

        AudioBitRate = new QLabel(layoutWidget3);
        AudioBitRate->setObjectName("AudioBitRate");

        verticalLayout_2->addWidget(AudioBitRate);

        layoutWidget4 = new QWidget(centralwidget);
        layoutWidget4->setObjectName("layoutWidget4");
        layoutWidget4->setGeometry(QRect(10, 390, 158, 26));
        horizontalLayout_4 = new QHBoxLayout(layoutWidget4);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        opendir = new QPushButton(layoutWidget4);
        opendir->setObjectName("opendir");

        horizontalLayout_4->addWidget(opendir);

        searchButton = new QPushButton(layoutWidget4);
        searchButton->setObjectName("searchButton");

        horizontalLayout_4->addWidget(searchButton);

        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName("stackedWidget");
        stackedWidget->setGeometry(QRect(140, 50, 400, 300));
        playlist = new QListView(centralwidget);
        playlist->setObjectName("playlist");
        playlist->setGeometry(QRect(10, 10, 141, 171));
        songlist = new QListView(centralwidget);
        songlist->setObjectName("musiclist");
        songlist->setGeometry(QRect(10, 220, 151, 161));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 750, 18));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        stackedWidget->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        playButton->setText(QCoreApplication::translate("MainWindow", "\346\222\255\346\224\276", nullptr));
        pushButton->setText(QCoreApplication::translate("MainWindow", "\346\232\202\345\201\234", nullptr));
        stopButton->setText(QCoreApplication::translate("MainWindow", "\345\201\234\346\255\242", nullptr));
        timeLabel->setText(QString());
        musicLength->setText(QString());
        delplaylist->setText(QCoreApplication::translate("MainWindow", "\345\210\240\351\231\244", nullptr));
        addplaylist->setText(QCoreApplication::translate("MainWindow", "\346\267\273\345\212\240", nullptr));
        autoplaycheckBox->setText(QCoreApplication::translate("MainWindow", "\350\207\252\345\212\250\346\222\255\346\224\276", nullptr));
        Title->setText(QCoreApplication::translate("MainWindow", "Title", nullptr));
        AlbumTitle->setText(QCoreApplication::translate("MainWindow", "AlbumTitle", nullptr));
        ContributingArtist->setText(QCoreApplication::translate("MainWindow", "ContributingArtist", nullptr));
        TrackNumber->setText(QCoreApplication::translate("MainWindow", "TrackNumber", nullptr));
        Date->setText(QCoreApplication::translate("MainWindow", "Date", nullptr));
        AudioBitRate->setText(QCoreApplication::translate("MainWindow", "AudioBitRate", nullptr));
        opendir->setText(QCoreApplication::translate("MainWindow", "\346\211\223\345\274\200", nullptr));
        searchButton->setText(QCoreApplication::translate("MainWindow", "\346\220\234\347\264\242", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
