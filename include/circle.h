#ifndef CIRCLE_H
#define CIRCLE_H

#include <QWidget>
#include <QTimer>
#include <QPainter>
using namespace std;

namespace Ui {
class circle;
}

class circle : public QWidget
{
    Q_OBJECT
public:
    explicit circle(QWidget *parent = nullptr);
    ~circle();
    void paintEvent(QPaintEvent *event);

private:
    Ui::circle *ui;
    int rotation;

signals:

public slots:
    void updaterRotation();
};

#endif // CMPROCESSBAR1_H
