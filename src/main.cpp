#include "mainwindow.h"
#include "string.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("体积视频播放器");
    w.show();
    return a.exec();
}
