#include "pco.h"
#include "openglwidget.h"
#include "parallel_encoder.h"
#include "parallel_decoder.h"
#include "config.h"

pco::pco(QObject *parent) : QObject(parent)
{

}

void pco::decoding(string file)
{
    QTime timedebuge;//声明一个时钟对象
    timedebuge.start();//开始计时

    parallel_decoder dec;
    dec.decoder("D:/design/design/result/longdress/auxiliary/aux_"+file + ".dat",
                "D:/design/design/result/longdress/geometry/geo_"+file + ".dat",
                "D:/design/design/result/longdress/color/color_"+file + ".jpg");
    qDebug()<<timedebuge.elapsed()/1000.0;//输出计时
    emit senddata(dec.getdata1());
    emit senddata(dec.getdata2());
}


