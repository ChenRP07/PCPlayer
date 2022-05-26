#ifndef PCO_H
#define PCO_H

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

class pco : public QObject
{
    Q_OBJECT
public:
    explicit pco(QObject *parent = nullptr);
    void decoding(string file);

signals:
    void senddata(vector<node>);

};

#endif // PCO_H
