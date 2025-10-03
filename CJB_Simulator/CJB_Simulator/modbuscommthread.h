#ifndef MODBUSCOMMTHREAD_H
#define MODBUSCOMMTHREAD_H


#include <QThread>
#include "mainwindow.h"
#include "externs.h"

extern MainWindow *mainAppWin;


class modbusCommThread : public QThread
{
    Q_OBJECT

protected:
    void run() override
    {
        modbusCommObj->initReadTimer();
        exec();
    }


signals:

};


#endif // MODBUSCOMMTHREAD_H
