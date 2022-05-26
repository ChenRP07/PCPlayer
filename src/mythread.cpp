#include "mythread.h"
#include "openglwidget.h"
#include "parallel_encoder.h"
#include "parallel_decoder.h"
#include "config.h"

mythread::mythread(QObject *parent) : QObject(parent)
{

}

void mythread::working(string file)
{
    QTime timedebuge;//声明一个时钟对象
    timedebuge.start();//开始计时

    vector<node> cloud;
    cloud.clear();
    HANDLE hSrcFile = CreateFileA(file.c_str(), GENERIC_READ, 0, NULL, OPEN_ALWAYS, 0, NULL);
    if(hSrcFile == INVALID_HANDLE_VALUE)
        return;
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
    char strLine[50] = {0};

    while (dwRemainSize > 0)
    {
        DWORD dwBlock = dwRemainSize < BUFFERBLOCKSIZE ? dwRemainSize : BUFFERBLOCKSIZE;
        __int64 qwFileOffset = dwFileSize - dwRemainSize;
        PBYTE pSrc = (PBYTE)MapViewOfFile(hSrcFileMapping, FILE_MAP_READ, (DWORD)(qwFileOffset >> 32), (DWORD)(qwFileOffset & 0xFFFFFFFF), dwBlock);
        PBYTE pSrcBak = pSrc;
        for (int i = 0; i < dwBlock; i++, pSrc++)
        {
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
                node temp(arrXYZ_RGB[0], arrXYZ_RGB[1], arrXYZ_RGB[2], arrXYZ_RGB[3]/255.0, arrXYZ_RGB[4]/255.0, arrXYZ_RGB[5]/255.0);
                cloud.push_back(temp);
                FieldCount = 0;//字段位置清零
                memset(strLine, 0, sizeof(strLine));//数字字符数组清空
            }
            else if ((*pSrc >= '0' && *pSrc <= '9'))
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
    qDebug()<<file.c_str()<<" "<<timedebuge.elapsed()/1000.0;//输出计时
    emit sendarry(cloud);
}


