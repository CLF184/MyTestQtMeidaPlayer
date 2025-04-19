#include "lyricview.h"
#include "ui_lyricview.h"

LyricView::LyricView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LyricView)
{
    ui->setupUi(this);
    
    // 初始化歌词标签
    ui->currentLyricLabel->setText("暂无歌词");
    ui->nextLyricLabel->setText("");
}

LyricView::~LyricView()
{
    delete ui;
}

void LyricView::updateLyric(const QString &currentLyric, const QString &nextLyric)
{
    if (currentLyric.isEmpty()) {
        ui->currentLyricLabel->setText("暂无歌词");
    } else {
        ui->currentLyricLabel->setText(currentLyric);
    }
    
    ui->nextLyricLabel->setText(nextLyric);
}
