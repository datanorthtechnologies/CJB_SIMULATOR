#ifndef CJBTHREAD_H
#define CJBTHREAD_H

#include <QThread>
#include "externs.h"

class cjbThread : public QThread
{
    Q_OBJECT

protected:
    void run() override
    {
        cjbfsmdidoObj = new cjbfsmdido();
        cjbfsmdidoObj->accessMenu();
        exec();
    }

signals:

};
#endif // CJBTHREAD_H
