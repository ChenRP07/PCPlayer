#include "circle.h"
#include "ui_circle.h"

circle::circle(QWidget *parent) :
    QWidget(parent),ui(new Ui::circle)
{
    ui->setupUi(this);
    QTimer *timer = new QTimer;
    timer->start(1);//定时3毫秒
    connect(timer,SIGNAL(timeout()),this,SLOT(updaterRotation()));// 定时旋转坐标系
    rotation = 0;
}

circle::~circle()
{
    delete ui;
}

void circle::updaterRotation(){ //循环360度旋转坐标系
    rotation += 10;
    if(rotation == 360){
        rotation = 0;
    }
    this->update();
}

void circle::paintEvent(QPaintEvent *event){//根据QPaintPath画出渐变色的圆弧
    int width = this->width();
    int height = this->height();
    int side = qMin(width, height);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.translate(width / 2, height / 2);
    painter.scale(side / 200.0, side / 200.0);

    QConicalGradient gra(QPoint(0,0),0);
    gra.setColorAt(0,QColor("#3BB6FE"));
    gra.setColorAt(1,QColor("#FFFFFF"));
    QBrush brush(gra);

    int radis = 40;
    int sider = 5;
    QRect rect(-radis,-radis,radis*2,radis*2);
    QPainterPath path;
    path.arcTo(rect,0, 270);

    QPainterPath subPath;
    subPath.addEllipse(rect.adjusted(sider, sider, -sider, -sider));

    path = path-subPath;
    painter.setBrush(brush);//QColor("#66CFFF")
    painter.setPen(Qt::NoPen);
    painter.rotate(rotation);
    painter.drawPath(path);


}


