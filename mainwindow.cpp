#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    player = new mymediaplayer();
    player->setVolume(50);
    ui->volumeSlider->setValue(50);

    playlists=new QList<QStringList>();
    QStringList temp;
    playlists->append(temp);
    QListWidgetItem *item = new QListWidgetItem(QString("Playlist %1").arg(++maxplaylistindex));
    ui->playlist->addItem(item);

    pic = new QLabel;
    pic->setScaledContents(true);
    video = new QVideoWidget;
    ui->stackedWidget->addWidget(pic);
    ui->stackedWidget->addWidget(video);
    player->setVideoOutput(video);



    connect(ui->volumeSlider,&QSlider::sliderPressed,this,&MainWindow::on_volumeSlider_sliderMoved);
    connect(ui->musicPostionSlider,&QSlider::sliderMoved,this,&MainWindow::on_musicPostionSlider_sliderPressed);

    connect(player, &QMediaPlayer::positionChanged, this, [this](int position) {
        position = player->position() / 1000;
        if(position>=ui->musicPostionSlider->maximum()&&autonext&&ui->musiclist->count()>ui->musiclist->currentRow()+1){
            path=QString(playlists->at(currentplaylistindex).at(ui->musiclist->currentRow()+1));
            player->setMedia(path);
            player->play();
        }
    });

    connect(ui->playlist, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        int index = ui->playlist->row(item);  // 获取选中的播放列表的索引
        if (index >= 0 && index < playlists->size()) {
            QStringList currentPlaylist = playlists->at(index);  // 获取该播放列表
            ui->musiclist->clear();  // 清空旧的歌曲列表
            // 将歌曲列表添加到 songListWidget 中
            for(int i=0;i<playlists->at(currentplaylistindex).size();i++){
                QFileInfo fileinfo(playlists->at(currentplaylistindex).at(i));
                ui->musiclist->addItem(fileinfo.fileName());
            }
        }
    });

    connect(player, &QMediaPlayer::metaDataChanged, this,[this]{
        ui->ContributingArtist->setText(player->metaData().value(QMediaMetaData::Key::ContributingArtist).toString());
        ui->AlbumTitle->setText(player->metaData().value(QMediaMetaData::Key::AlbumTitle).toString());
        ui->Date->setText(player->metaData().value(QMediaMetaData::Key::Date).toString());
        ui->AudioBitRate->setText(player->metaData().value(QMediaMetaData::Key::AudioBitRate).toString());
        ui->TrackNumber->setText(player->metaData().value(QMediaMetaData::Key::TrackNumber).toString());
        ui->Title->setText(player->metaData().value(QMediaMetaData::Key::Title).toString());
        if(player->hasVideo()){
            //ui->stackedWidget->resize(player->player->metaData().value(QMediaMetaData::Resolution).toSize());
            ui->stackedWidget->setCurrentIndex(ui->stackedWidget->indexOf(video));
        }
        else{
            ui->stackedWidget->setCurrentIndex(ui->stackedWidget->indexOf(pic));
            QImage image = player->metaData().value(QMediaMetaData::Key::ThumbnailImage).value<QImage>();
            pic->setPixmap(QPixmap::fromImage(image));
        }
    });

    // 更新进度条和时间标签
    connect(player, &QMediaPlayer::positionChanged, this, [this](int position) {
        position = player->position() / 1000;
        ui->musicPostionSlider->setValue(position);
        ui->timeLabel->setNum((int)position);
    });

    // 设置进度条范围
    connect(player, &QMediaPlayer::durationChanged, this, [this](qint64 max) {
        ui->musicPostionSlider->setRange(0, max / 1000);  // 设置进度条的最大值
        ui->musicLength->setNum((int)max / 1000);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_playButton_clicked()
{
    player->play();
}

void MainWindow::on_stopButton_clicked()
{
    player->stop();
}

void MainWindow::on_volumeSlider_sliderMoved()
{
    player->setVolume(ui->volumeSlider->value());
}

void MainWindow::on_searchButton_clicked()
{
    QtConcurrent::run([=](){
        QStringList filepaths;
        QDirIterator it(express, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QString filePath = it.next();
            QFileInfo fileInfo(filePath);
            if (fileInfo.isFile() && extensions.contains(fileInfo.suffix(), Qt::CaseInsensitive)) {
                filepaths.append(fileInfo.filePath());
                qDebug()<<filePath;
            }
        }
        // 回到主线程更新 UI
        QMetaObject::invokeMethod(this, [=](){
            playlists->replace(currentplaylistindex, filepaths);
            ui->musiclist->clear();  // 清空旧的搜索结果
            for (const QString &file : playlists->at(currentplaylistindex)) {
                ui->musiclist->addItem(QFileInfo(file).fileName());
            }
        }, Qt::QueuedConnection);
    });
}

void MainWindow::on_musiclist_itemDoubleClicked()
{
    QString filePath = playlists->at(currentplaylistindex).at(ui->musiclist->currentRow());  // 获取完整路径
    player->setMedia(filePath);
    player->play();
}

void MainWindow::on_pushButton_clicked()
{
    player->pause();
}

void MainWindow::on_musiclist_itemClicked()
{
    QString filePath = playlists->at(currentplaylistindex).at(ui->musiclist->currentRow());  // 获取完整路径
    player->setMedia(filePath);
}

void MainWindow::on_addplaylist_clicked()
{
    QStringList *temp=new QStringList();
    playlists->append(*temp);
    QListWidgetItem *item = new QListWidgetItem(QString("Playlist %1").arg(++maxplaylistindex));
    ui->playlist->addItem(item);  // 将播放列表项添加到列表中
}

void MainWindow::on_playlist_currentRowChanged(int currentRow)
{
    currentplaylistindex=currentRow;
}


void MainWindow::on_delplaylist_clicked()
{
    int row = ui->playlist->currentRow(); // 获取当前选中的行号
    if (row >= 0 && row < playlists->size()) { // 确保行号有效
        playlists->removeAt(row);  // 删除对应的数据
        delete ui->playlist->takeItem(row);  // 删除 QListWidget 里的项目，避免内存泄漏
    }
}


void MainWindow::on_autoplaycheckBox_checkStateChanged(const Qt::CheckState &arg1)
{
    autonext=arg1;
}

void MainWindow::on_musicPostionSlider_sliderPressed()
{
    player->setfilePosition(ui->musicPostionSlider->sliderPosition());
}

void MainWindow::on_opendir_clicked()
{
    express=QFileDialog::getExistingDirectory(this, tr("Open Directory"),"", QFileDialog::ShowDirsOnly);
}
