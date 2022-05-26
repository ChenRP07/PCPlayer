#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <GL/glu.h>
#include <QThreadPool>
#include <string>
#include <QEventLoop>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QOpenGLShaderProgram>
#include <QTime>
#include <QTimer>
#include <QKeyEvent>
#include <QFileDialog>
#include <math.h>
#include <QMessageBox>
#include <QMap>
#include <QOpenGLBuffer>
#include <synchapi.h>
#include "Octree.h"
#include "Octree2.h"
#include "mythread.h"
using namespace std;

namespace Ui {
class openglwidget;
}

class openglwidget : public QOpenGLWidget, QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    explicit openglwidget(QWidget *parent = 0);
    ~openglwidget();
    void run();
    void stop();
    void getlist();
    void receiveplay();
    void clearbuffer();
    void draw(int number, QThread *t);
    void receivedir(QString filename);
    void load(vector<vector<node>> cloud, int w);
    QStringList getFileNames(const QString &path);
    void drawCube(Octree2::Octree_Struct octree, int drawDepth);
    void drawCube2(Octree2::Octree_Struct octree, int drawDepth);

    int steps = 1;
    bool version = false;


private:
    Ui::openglwidget *ui;
    float translatez;

    bool lPressed = false;
    bool rPressed = false;

    QPoint pressPoint;
    QPoint movePoint;
    float moveRate = 1.0;

    float moveRad_x = 0;
    float moveRad_y = 0;
    float moveLength_x = 0;
    float moveLength_y = 0;
    float cloud_size = 0;
    int showDepth = 1;

    bool move = false;
    bool showCube = false;
    bool showCubeLine = false;
    bool showAllCubeLine = false;


    QTimer *timer;
    QStringList dir;
    QString dirpath;
    QEventLoop loop;
    bool openFile;
    int num = 0;
    int count = 0;
    int speed = 30;
    int thread_count = 3;
    float result;
    QOpenGLShaderProgram *sharedprogram;
    //操作View，我这里为了展示没有封装成单独的Camera类
    //Camera Attributes
    QVector3D cameraPos{ 0.0f, 0.0f, 3.0f };
    QVector3D cameraFront{ 0.0f, 0.0f, -1.0f };
    QVector3D cameraUp{ 0.0f, 1.0f, 0.0f };

    vector<QOpenGLBuffer *> showlist;
    vector<QThread *> threads;

    Octree2 oct;



protected:
    void initializeGL() override;
    void resizeGL(int w, int h)override;
    void paintGL()override;

    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent * event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;
    void keyPressEvent(QKeyEvent *event)override;


signals:
    void starting(string name);
    void test(int number);
    void increase(int);
    void step();

public slots:
    void slotSqlResult(vector<node>);
    void getcount(int);
    void getout();
    void getspeed(int);
};

#endif // OPENGLWIDGET_H
