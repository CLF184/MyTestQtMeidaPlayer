#ifndef LYRICVIEW_H
#define LYRICVIEW_H

#include <QWidget>
#include <QLabel>

namespace Ui {
class LyricView;
}

class LyricView : public QWidget
{
    Q_OBJECT

public:
    explicit LyricView(QWidget *parent = nullptr);
    ~LyricView();

public slots:
    // 更新当前显示的歌词
    void updateLyric(const QString &currentLyric, const QString &nextLyric);

private:
    Ui::LyricView *ui;
};

#endif // LYRICVIEW_H
