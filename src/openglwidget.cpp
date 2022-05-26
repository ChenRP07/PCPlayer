#include "openglwidget.h"
#include "ui_openglwidget.h"
#include "parallel_decoder.h"
#include "pco.h"

vector<bool> getshowlist;

openglwidget::openglwidget(QWidget *parent) :
    QOpenGLWidget(parent),
    ui(new Ui::openglwidget)
{
    qRegisterMetaType<string>("string");
    qRegisterMetaType<vector<node>>("vector<node>");
    setFocusPolicy(Qt::ClickFocus);

    this->thread()->setPriority(QThread::HighestPriority);
    showlist.resize(300);

    timer = new QTimer(this);
    threads.resize(thread_count);
    for(int i=0;i<thread_count;i++)
        threads[i] = new QThread();
    ui->setupUi(this);
    count = 0;

    //空间八叉树划分
    oct.octLength = 1;
}

openglwidget::~openglwidget()
{
    makeCurrent();//调用当前状态 不用管为什么，加就好了
    doneCurrent();//改变当前状态 不用管为什么，加就好了
    delete ui;
}

void openglwidget::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);

    sharedprogram = new QOpenGLShaderProgram(this);
    sharedprogram->addShaderFromSourceFile(QOpenGLShader::Vertex,"D:/design/design/shapes.vert");
    sharedprogram->addShaderFromSourceFile(QOpenGLShader::Fragment,"D:/design/design/shapes.frag");
    sharedprogram->link();
}

void openglwidget::paintGL()
{
    glClearColor(0, 0, 0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    sharedprogram->bind();
    double r = translatez;
    QVector3D pos =QVector3D(r*cos(3.14*(moveRad_y / 180)), r*sin(3.14*(moveRad_x / 180)), r*sin(3.14*(moveRad_y / 180))) + cameraFront;
    cameraPos = pos;

    //视角设置
    QMatrix4x4 view;
    view.translate(moveLength_x, -moveLength_y, 0.0f);
    view.lookAt(cameraPos, cameraFront, cameraUp);
    //透视投影
    QMatrix4x4 projection;
    projection.perspective(45, 1.0f * width() / height(), 0.1f, 40000.0f);
    sharedprogram->setUniformValue("trans", projection * view);

    if(showlist[count]!=NULL)
    {
        showlist[count]->bind();
        cloud_size = showlist[count]->size()/24;
        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glPointSize(2.0f);
        glDrawArrays(GL_POINTS,0,cloud_size);

        showlist[count]->release();
    }
    sharedprogram->release();
}

void openglwidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void openglwidget::draw(int number, QThread *t)
{
    if(!version)
    {
        mythread *work = new mythread();
        QString name = QString::number(number);
        work->setObjectName(name);
        work->moveToThread(t);
        connect(this, &openglwidget::starting, work, &mythread::working);
        QString fileName = dirpath +"/"+ dir[number];
        string filepath = fileName.toStdString();
        emit starting(filepath);
        disconnect(this, &openglwidget::starting, work, &mythread::working);
        connect(work, SIGNAL(sendarry(vector<node>)), this, SLOT(slotSqlResult(vector<node>)));
    }
    else
    {
        pco *work = new pco();
        QString name = QString::number(number);
        work->setObjectName(name);
        work->moveToThread(t);
        connect(this, &openglwidget::starting, work, &pco::decoding);
        string filepath = to_string(number+1050);
        emit starting(filepath);
        disconnect(this, &openglwidget::starting, work, &pco::decoding);
        connect(work, SIGNAL(senddata(vector<node>)), this, SLOT(slotSqlResult(vector<node>)));
    }
    t->start();
    t->setPriority(QThread::HighestPriority);
}


void openglwidget::slotSqlResult(vector<node> data)
{
    QOpenGLBuffer *temp;
    temp = new QOpenGLBuffer();
    temp->create();
    temp->bind();
    temp->allocate(&data[0], data.size()*6*sizeof(float));
    temp->release();
    data.clear();
    int index = sender()->objectName().toInt();
    if(getshowlist[index])
        index++;
    if(showlist[index]==NULL)
        emit step();//避免被上一个视频信号影响
    showlist[index] = temp;
    getshowlist[index] = true;
    if(num<60)
    {
        if(version)
        {
            if(num%2==0)
                num--;
        }
        num += steps;
        draw(num, sender()->thread());
    }
}

void openglwidget::load(vector<vector<node> > data, int w)
{
   /* vector<QOpenGLBuffer *> load;
    load.resize(data.size());
    for(int i=0;i<data.size();i++)
    {
        QOpenGLBuffer *temp;
        temp = new QOpenGLBuffer();
        temp->create();
        temp->bind();
        temp->allocate(&data[i][0], data[i].size()*6*sizeof(float));
        load[i] = temp;
        temp->release();
        data[i].clear();
    }

    showlist[w] = load;*/
}

void openglwidget::getcount(int index)
{
    if(loop.isRunning())
        loop.exit();
    count = index;
}

QStringList openglwidget::getFileNames(const QString &path)
{
    QDir dir(path);
    QStringList nameFilters;
    nameFilters << "*.ply";
    QStringList files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);
    return files;
}

void openglwidget::wheelEvent(QWheelEvent *event)
{
    if (event->delta() > 0) {
        translatez /= 1.1;
    }
    else if (event->delta() < 0) {
        translatez *= 1.1;
    }
    this->update();
}

void openglwidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        lPressed = true;
    }
    else if (event->button() == Qt::RightButton) {
        rPressed = true;
    }
    pressPoint = event->pos();
}

void openglwidget::mouseMoveEvent(QMouseEvent *event)
{
    if (lPressed == true) {
        movePoint = event->pos();
        moveRad_y += (movePoint.x() - pressPoint.x()) / moveRate;
        moveRad_x += (movePoint.y() - pressPoint.y()) / moveRate;
        pressPoint = movePoint;

        if (moveRad_x > 90)moveRad_x = 90;
        if (moveRad_x < -90)moveRad_x = -90;

        this->update();
    }
    if (rPressed == true) {
        movePoint = event->pos();
        moveLength_x += 2*(movePoint.x() - pressPoint.x());
        moveLength_y += 2*(movePoint.y() - pressPoint.y()) ;
        pressPoint = movePoint;

        this->update();
    }
}

void openglwidget::mouseReleaseEvent(QMouseEvent *event)
{
    lPressed = false;
    rPressed = false;
}

void openglwidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Plus:
        if (showDepth + 1 <= oct.root->level) {
            showDepth += 1;
            update();
        }
        break;
    case Qt::Key_Minus:
        if (showDepth - 1 >= 1) {
            showDepth -= 1;
            update();
        }
        break;
    case Qt::Key_1:
        showCube = !showCube;
        update();
        break;
    case Qt::Key_2:
        showCubeLine = !showCubeLine;
        update();
        break;
    case Qt::Key_3:
        showAllCubeLine = !showAllCubeLine;
        update();
        break;
    }
}

void openglwidget::clearbuffer()
{
    //删除上一个播放视频的缓存数据
    for(int i=0;i<showlist.size();i++)
    {
        if(showlist[i]!=NULL)
            showlist[i]->destroy();
    }
    showlist.clear();
    getshowlist.clear();
    this->update();
}

void openglwidget::receivedir(QString filename)
{

    oct.root = NULL;
    if(!version)
        steps = 1;
    else
        steps = 2;

    QTime timedebuge;//声明一个时钟对象
    timedebuge.start();//开始计时
    //加载新一个播放视频的帧数据
    dirpath =  filename;
    dir = getFileNames(dirpath);
    showlist.resize(dir.size());
    getshowlist.resize(dir.size(), false);

    int first = 0;
    QVector3D maxPoint;
    QVector3D minPoint;
    vector<node> cloud;
    QString fileName = dirpath +"/"+ dir[0];
    qDebug()<<fileName;
    string filepath = fileName.toStdString();
    HANDLE hSrcFile = CreateFileA(filepath.c_str(), GENERIC_READ, 0, NULL, OPEN_ALWAYS, 0, NULL);
    if(hSrcFile == INVALID_HANDLE_VALUE)
    {
        qDebug()<<"文件打开失败";
        return;
    }

    LARGE_INTEGER tInt2;
    GetFileSizeEx(hSrcFile, &tInt2);
    __int64 dwRemainSize = tInt2.QuadPart;
    __int64 dwFileSize = dwRemainSize;
    HANDLE hSrcFileMapping = CreateFileMapping(hSrcFile, NULL, PAGE_READONLY, tInt2.HighPart, tInt2.LowPart, NULL);
    if (hSrcFileMapping == INVALID_HANDLE_VALUE)
    {
        qDebug()<<"文件打开失败";
        return;
    }
    SYSTEM_INFO SysInfo;
    GetSystemInfo(&SysInfo);
    DWORD dwGran = SysInfo.dwAllocationGranularity;
    const int BUFFERBLOCKSIZE = dwGran * 1024;

    int FieldIndex = 0;//每一个小数字的填充位置
    int FieldCount = 0;//每一行中整数字位置

    float arrXYZ_RGB[6];
    char strLine[1024] = { 0 };
    while (dwRemainSize > 0)
    {
        DWORD dwBlock = dwRemainSize < BUFFERBLOCKSIZE ? dwRemainSize : BUFFERBLOCKSIZE;
        __int64 qwFileOffset = dwFileSize - dwRemainSize;
        PBYTE pSrc = (PBYTE)MapViewOfFile(hSrcFileMapping, FILE_MAP_READ,
                                          (DWORD)(qwFileOffset >> 32), (DWORD)(qwFileOffset & 0xFFFFFFFF), dwBlock);
        PBYTE pSrcBak = pSrc;
        for (int i = 0; i < dwBlock; i++, pSrc++)
        {
            //这样的处理方式有一个很大的缺点
            //当整个文件的最后一行不是空一行的话，整个数据会少一行。
            //但是一般默认情况下整个数据的最后一行是有一个换行的
            if (*pSrc == '\n')
            {
                //整行读完了====================================================
                if (FieldIndex != 0)//先处理一次字段。
                {
                    strLine[FieldIndex] = '\0';//在末尾处加上符号。
                    arrXYZ_RGB[FieldCount++] = atof(strLine);
                    FieldIndex = 0;
                }
                if(FieldCount!=6)
                {
                    FieldCount = 0;//字段位置清零
                    memset(strLine, 0, sizeof(strLine));//数字字符数组清空
                    continue;
                }
                float x, y, z;
                node temp(arrXYZ_RGB[0], arrXYZ_RGB[1], arrXYZ_RGB[2], arrXYZ_RGB[3]/255.0, arrXYZ_RGB[4]/255.0, arrXYZ_RGB[5]/255.0);
                cloud.push_back(temp);
                x = arrXYZ_RGB[0];
                y = arrXYZ_RGB[1];
                z = arrXYZ_RGB[2];
                //oct.insertPoint(oct.root, Octree2::Point(x, y, z));
                if (first == 0)
                {
                    maxPoint = minPoint = QVector3D(x, y, z);
                    first++;
                }
                else
                {
                    maxPoint.setX(maxPoint.x() < x ? x : maxPoint.x());
                    maxPoint.setY(maxPoint.y() < y ? y : maxPoint.y());
                    maxPoint.setZ(maxPoint.z() < z ? z : maxPoint.z());

                    minPoint.setX(minPoint.x() > x ? x : minPoint.x());
                    minPoint.setY(minPoint.y() > y ? y : minPoint.y());
                    minPoint.setZ(minPoint.z() > z ? z : minPoint.z());
                }
                FieldCount = 0;//字段位置清零
                memset(strLine, 0, sizeof(strLine));//数字字符数组清空
            }
            else if ((*pSrc >= '0' && *pSrc <= '9')||*pSrc=='.')
            {
                //空格或Tab
                strLine[FieldIndex++] = *pSrc;
            }
            else if(*pSrc ==' ')
            {
                //此时为行内分割===关键是连续几次无用字符==============================
                if (FieldIndex != 0)
                {
                    //一个字段处理完毕
                    strLine[FieldIndex] = '\0';
                    arrXYZ_RGB[FieldCount++] = atof(strLine);
                    FieldIndex = 0;
                }
            }
        }
        UnmapViewOfFile(pSrcBak);
        dwRemainSize -= dwBlock;
    }
    CloseHandle(hSrcFileMapping);
    CloseHandle(hSrcFile);
    qDebug()<<timedebuge.elapsed()/1000.0;//输出计时
    double maxlenth = sqrt(pow(maxPoint.x() - minPoint.x(), 2) + pow(maxPoint.y() - minPoint.y(), 2) + pow(maxPoint.z() - minPoint.z(), 2));
    translatez = maxlenth*2.5;
    QVector3D midpoint = QVector3D((maxPoint.x() - minPoint.x()) / 2, (maxPoint.y() - minPoint.y()) / 2, (maxPoint.z()- minPoint.z() / 2));
    cameraFront = midpoint;

    QOpenGLBuffer *temp;
    temp = new QOpenGLBuffer();
    temp->create();
    temp->bind();
    temp->allocate(&cloud[0], cloud.size()*6*sizeof(float));
    temp->release();
    cloud.clear();

    showlist[0] = temp;
    getshowlist[0] = true;
    this->update();
    /*num = 1051;
    for(int i=0;i<30;i=i+2)
    {
        parallel_decoder dec;
        dec.decoder("D:/design/design/result/longdress/auxiliary/aux_"+to_string(num) + ".dat",
                    "D:/design/design/result/longdress/geometry/geo_"+to_string(num) + ".dat",
                    "D:/design/design/result/longdress/color/color_"+to_string(num) + ".jpg");
        num += 2;
        this->load(dec.getdata1(), i);
        this->load(dec.getdata2(), i+1);
        qDebug()<<timedebuge.elapsed()/1000.0;//输出计时
    }*/
    num = -1;
    for(int i=0;i<thread_count;i++)
    {
        num += steps;
        draw(num, threads[i]);
    }
}

void openglwidget::drawCube2(Octree2::Octree_Struct octree, int drawDepth)
{
    if (octree->level > drawDepth) {
        for (int i = 0; i < 8; i++) {
            if (octree->nodes[i] != NULL) {
                drawCube2(octree->nodes[i], drawDepth);
            }
        }
    }
    else {
        double x = octree->centel.x;
        double y = octree->centel.y;
        double z = octree->centel.z;
        double length = octree->length;
        Octree2::Point p[8];
        p[0].setPoint(x + length, y + length, z + length);
        p[1].setPoint(x - length, y + length, z + length);
        p[2].setPoint(x - length, y - length, z + length);
        p[3].setPoint(x + length, y - length, z + length);

        p[4].setPoint(x + length, y + length, z - length);
        p[5].setPoint(x - length, y + length, z - length);
        p[6].setPoint(x - length, y - length, z - length);
        p[7].setPoint(x + length, y - length, z - length);
        int cubeIndexs[6][4] = { {3,2,1,0},
                                {4,5,6,7},
                                {0,1,5,4},
                                {1,2,6,5},
                                {2,3,7,6},
                                {3,0,4,7} };

        for (int i = 0; i < 6; i++) {
            glBegin(GL_POLYGON);
            for (int j = 0; j < 4; j++) {
                glVertex3f(p[cubeIndexs[i][j]].x, p[cubeIndexs[i][j]].y, p[cubeIndexs[i][j]].z);
            }
            glEnd();
        }

    }
}

void openglwidget::drawCube(Octree2::Octree_Struct octree, int drawDepth)
{
    if (octree->level > drawDepth) {
        for (int i = 0; i < 8; i++) {
            if (octree->nodes[i] != NULL) {
                drawCube(octree->nodes[i], drawDepth);
            }
        }
    }
    else {
        double x = octree->centel.x;
        double y = octree->centel.y;
        double z = octree->centel.z;
        double length = octree->length;

        Octree2::Point p1(x + length, y + length, z + length);
        Octree2::Point p2(x - length, y + length, z + length);
        Octree2::Point p3(x - length, y - length, z + length);
        Octree2::Point p4(x + length, y - length, z + length);

        Octree2::Point p5(x + length, y + length, z - length);
        Octree2::Point p6(x - length, y + length, z - length);
        Octree2::Point p7(x - length, y - length, z - length);
        Octree2::Point p8(x + length, y - length, z - length);
        //top
        glVertex3f(p1.x, p1.y, p1.z);
        glVertex3f(p2.x, p2.y, p2.z);
        glVertex3f(p2.x, p2.y, p2.z);
        glVertex3f(p3.x, p3.y, p3.z);
        glVertex3f(p3.x, p3.y, p3.z);
        glVertex3f(p4.x, p4.y, p4.z);
        glVertex3f(p4.x, p4.y, p4.z);
        glVertex3f(p1.x, p1.y, p1.z);
        //bottom
        glVertex3f(p5.x, p5.y, p5.z);
        glVertex3f(p6.x, p6.y, p6.z);
        glVertex3f(p6.x, p6.y, p6.z);
        glVertex3f(p7.x, p7.y, p7.z);
        glVertex3f(p7.x, p7.y, p7.z);
        glVertex3f(p8.x, p8.y, p8.z);
        glVertex3f(p8.x, p8.y, p8.z);
        glVertex3f(p5.x, p5.y, p5.z);

        glVertex3f(p1.x, p1.y, p1.z);
        glVertex3f(p5.x, p5.y, p5.z);
        glVertex3f(p2.x, p2.y, p2.z);
        glVertex3f(p6.x, p6.y, p6.z);
        glVertex3f(p3.x, p3.y, p3.z);
        glVertex3f(p7.x, p7.y, p7.z);
        glVertex3f(p4.x, p4.y, p4.z);
        glVertex3f(p8.x, p8.y, p8.z);
    }
}

void openglwidget::stop()
{
    if(loop.isRunning())
        loop.exit();
    timer->stop();
}

void openglwidget::getspeed(int number)
{
    if(loop.isRunning())
        loop.exit();
    switch(number){
        case 1:{
            speed = 60;
            break;
        }
        case 2:{
            speed = 30;
            break;
        }
        case 4:{
            speed = 15;
            break;
        }
    }
    if(timer->isActive())
        timer->start(speed);
}

void openglwidget::getout()
{
    this->update();
    loop.exit();
}

void openglwidget::receiveplay()
{
    QTime timedebuge;//声明一个时钟对象
    timedebuge.start();//开始计时
    connect(timer, SIGNAL(timeout()), this, SLOT(getout()));
    timer->start(speed);
    int t = 1;
    if(version&&speed==15)
        t = 2;
    while(count<dir.size())
    {
        if(!timer->isActive())
            return;
        loop.exec();
        emit increase(count);
        count += t;
        while(count<dir.size()&&showlist[count]==NULL)
            count++;
    }
    count = 0;
    qDebug()<<timedebuge.elapsed()/1000.0;//输出计时
    timer->stop();
    /*while(index<dir.size())
    {
        it = val.find(index);
        if(it==val.end())
        {
            Sleep(30);
            continue;
        }
        oct.pointCloud.clear();
        oct.pointCloud = it.value();
        val.erase(it);
        connect(timer, SIGNAL(timeout()), this, SLOT(update()));
        loop.exec();
        index++;
    }
    timer->stop();*/
}
