#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "playlistmodel.h"
#include "openglwidget.h"
#include "parallel_decoder.h"
#include "qdebug.h"
#include "qtime"
#include<QKeyEvent>
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->Circle->setVisible(false);
    ui->label->setVisible(false);
    ui->horizontalSlider_2->setValue(2);
    ui->listWidget->setResizeMode(QListView::Adjust);
    ui->listWidget->setIconSize(QSize(100,100));
    ui->horizontalSlider->setMinimum(0);
    ui->horizontalSlider->setTracking(false);
    ui->horizontalSlider_2->setTracking(false);

    ui->horizontalSlider->installEventFilter(this);
    //设置listWidget样式表
    ui->listWidget->setStyleSheet("QListWidget{background-color:rgb(33, 33, 33); border:0px; color:white; outline:0px }"
                               "QListWidget::Item{padding-top:10px; padding-bottom:10px; }"
                               "QListWidget::Item:hover{background:solid gray; }"
                               "QListWidget::item:selected{background:rgb(40, 40, 40); color:rgb(0, 241, 241); }"
                               );

    connect(ui->widget,SIGNAL(increase(int)),this,SLOT(change(int)));
    connect(ui->widget,SIGNAL(step()),this,SLOT(load()));
    connect(ui->file_open,SIGNAL(clicked(bool)),this, SLOT(on_openfile_clicked()));
    connect(this, SIGNAL(sendcount(int)), ui->widget, SLOT(getcount(int)));
    connect(this, SIGNAL(sendspeed(int)), ui->widget, SLOT(getspeed(int)));

    this->thread()->setPriority(QThread::HighestPriority);

    w = ui->widget->width();
    h = ui->widget->height();

    v1 = (float)ui->openfile->geometry().x()/w, v2 = (float)ui->openfile->geometry().y()/h;

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::load()
{
    process += (float)100.0/150.0;
    int ret = (int)(process + 0.5f);//加 0.5 向下取整
    QString str = QString::number(ret);
    str = str+"%";
    ui->label->setText(str);
    if(ret==100)
    {
        ui->label->setVisible(false);
        ui->Circle->setVisible(false);
    }

}

void MainWindow::change(int list)
{
    index = list;
    ui->horizontalSlider->setValue(index);
    if(index>=ui->horizontalSlider->maximum())
        ui->openfile->show();
}

void MainWindow::warningshow(QString data)
{
    QMessageBox box;
    box.setText(data);
    box.exec();
}

void MainWindow::on_openfile_clicked()
{
    ui->horizontalSlider->setValue(0);//进度条恢复初值
    ui->openfile->hide();
    ui->Circle->setVisible(false);
    ui->label->setVisible(false);

    ui->label->setText("0%");
    QFont ft;
    ft.setPointSize(12);
    ui->label->setFont(ft);
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::white);
    ui->label->setPalette(pa);
    ui->label->setAttribute(Qt::WA_TranslucentBackground);

    QString name = QFileDialog::getExistingDirectory(NULL,"/design/tree","./",QFileDialog::ShowDirsOnly);
    if(name=="")
    {
        warningshow("未选择文件");
        ui->openfile->show();
        return;
    }
    QString str = name;
    QDir dir(name);
    QStringList nameFilters;
    nameFilters << "*.ply";
    QStringList files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);
    if(!files.size())
    {
        warningshow("未找到指定格式的视频文件");
        ui->openfile->show();
        return;
    }
    ui->horizontalSlider->setMaximum(files.size());
    QListWidgetItem *list_item = new QListWidgetItem(QIcon(":/myImage/image/list.png"),
                                                         str);
    for(int i=0;i<ui->listWidget->count();i++)
    {
        QListWidgetItem *temp = ui->listWidget->item(i);
        if(name == temp->text())
           ui->listWidget->takeItem(i);
    }
    ui->listWidget->insertItem(0, list_item);
    ui->listWidget->setCurrentRow(0);//设置选中当条目
    ui->widget->clearbuffer();
    ui->widget->receivedir(name);
    process = 0;
    ui->Circle->setVisible(true);
    ui->label->setVisible(true);
}


void MainWindow::on_play_clicked()
{
    int ret = (int)(process + 0.5f);//加 0.5 向下取整
    //if(ret!=100)
        //return;
    if(run)
    {
        ui->Circle->setVisible(false);
        ui->label->setVisible(false);
        ui->openfile->hide();
        ui->play->setStyleSheet("border-image:url(:/myImage/image/run.png)");
        run = false;
        ui->widget->receiveplay();
        ui->play->setStyleSheet("border-image:url(:/myImage/image/play.png)");
        run = true;
    }
    else
    {
        ui->play->setStyleSheet("border-image:url(:/myImage/image/play.png)");
        ui->widget->stop();
        run = true;
    }
}

void MainWindow::on_next_clicked()
{
    int waitshow = showcount - 1;
    if(waitshow < 0)
    {
        warningshow("当前视频即为最后一个视频 ");
    }
    else
    {
        showcount--;
        QListWidgetItem *temp = ui->listWidget->itemAt(waitshow, 0);
        process = 0;
        ui->label->setText("0%");
        ui->openfile->hide();
        ui->widget->clearbuffer();
        ui->widget->receivedir(temp->text());
        ui->Circle->setVisible(true);
        ui->label->setVisible(true);
    }

}

void MainWindow::on_previous_clicked()
{
    int waitshow = showcount + 1;
    if(waitshow >= ui->listWidget->count())
    {
        warningshow("当前视频即为第一个视频");
    }
    else
    {
        showcount++;
        QListWidgetItem *temp = ui->listWidget->itemAt(waitshow, 0);
        process = 0;
        ui->label->setText("0%");
        ui->openfile->hide();
        ui->widget->clearbuffer();
        ui->widget->receivedir(temp->text());
        ui->Circle->setVisible(true);
        ui->label->setVisible(true);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Escape:
        this->showNormal();
        break;
    }
}


void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    index =  ui->horizontalSlider->value();
    emit sendcount(index);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    ui->openfile->move(ui->widget->width()*v1, ui->widget->height()*v2);
    ui->Circle->move(ui->widget->width()*v1, ui->widget->height()*v2);
    ui->label->move(ui->widget->width()*v1+31, ui->widget->height()*v2+14);
}

void MainWindow::on_listWidget_doubleClicked(const QModelIndex &index)
{
    ui->label->setText("0%");
    ui->openfile->hide();
    ui->Circle->setVisible(true);
    ui->label->setVisible(true);
    ui->widget->clearbuffer();
    ui->widget->receivedir(index.data().toString());
    process = 0;
    showcount = index.row();
}

void MainWindow::on_horizontalSlider_2_valueChanged(int value)
{
    index =  ui->horizontalSlider_2->value();
    if(index==3)
        index++;
    float s = index*0.5;
    ui->speed->setText(QString::number(s ,'f',1));
    emit sendspeed(index);
}

void MainWindow::on_fullscreen_clicked()
{
    if(!fullscreen)
    {
        this->showFullScreen();
        fullscreen = true;
    }
    else
        this->showNormal();
}

void MainWindow::on_stop_clicked()
{
    //if(ret!=100)
        //return;
    ui->Circle->setVisible(false);
    ui->label->setVisible(false);
    ui->openfile->show();
    ui->widget->clearbuffer();
}


bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    //解决QSlider点击不能到鼠标指定位置的问题
    if(obj==ui->horizontalSlider)
    {
        if (event->type()==QEvent::MouseButtonPress)           //判断类型
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton)	//判断左键
            {
               int value = QStyle::sliderValueFromPosition(ui->horizontalSlider->minimum(), ui->horizontalSlider->maximum(), mouseEvent->pos().x(), ui->horizontalSlider->width());
               ui->horizontalSlider->setValue(value);
               emit sendcount(value);
            }
        }
    }
    return QObject::eventFilter(obj,event);
}


void MainWindow::on_pushButton_clicked()
{
    ui->widget->version = !ui->widget->version;
    if(ui->widget->version)
        ui->widget->steps = 2;
    else
        ui->widget->steps = 1;
    if(!ui->widget->version)
        ui->pushButton->setText("正常");
    else
        ui->pushButton->setText("压缩");
}
