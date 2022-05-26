#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#pragma execution_character_set("utf-8")

#include <QStringListModel>
#include <string>
#include <QMediaPlaylist>
#include <QStandardItemModel>
#include <QMessageBox>
#include <mutex>
#include "openglwidget.h"
using namespace std;


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void warningshow(QString data);

public slots:
    void on_openfile_clicked();
    void on_play_clicked();
    void on_next_clicked();
    void on_previous_clicked();    
    void on_horizontalSlider_valueChanged(int value);
    void on_listWidget_doubleClicked(const QModelIndex &index);

protected:
    void keyPressEvent(QKeyEvent * event)override;
    void resizeEvent(QResizeEvent *event) override;
    virtual bool eventFilter(QObject *obj, QEvent *event);


private:
    Ui::MainWindow *ui;
    QMediaPlaylist *m_playlist = nullptr;
    QStringListModel *Model;
    QStandardItemModel *ItemModel;
    bool run = true;
    float h, w, v1, v2, process = 0;

    bool fullscreen = false;
    bool press = false;
    int index = 0, mode = 0;
    int showcount = 0;//记录当前播放的是播放列表中的哪一个
    mutex process_load;


signals:
    void sendcount(int);
    void sendspeed(int);

public slots:
    void change(int);
    void load();
private slots:
    void on_horizontalSlider_2_valueChanged(int value);
    void on_fullscreen_clicked();
    void on_stop_clicked();
    void on_pushButton_clicked();
};
#endif // MAINWINDOW_H
