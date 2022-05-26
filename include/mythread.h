#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QObject>
#include <QThread>
#include <string>
#include <QMutex>
#include <string.h>
#include <string>
#include <sstream>
#include <iostream>
#include <QSemaphore>
#include "Octree.h"
using namespace std;

class mythread : public QObject
{
    Q_OBJECT
public:
    explicit mythread(QObject *parent = nullptr);
    void working(string file);

signals:
    void sendarry(vector<node>);

};

#endif // MYTHREAD_H
