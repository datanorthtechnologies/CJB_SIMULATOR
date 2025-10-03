#include <QApplication>
#include "mainwindow.h"
#include "cjbthread.h"
#include "modbuscommthread.h"
#include "globals.h"

MainWindow* mainAppWin;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    mainAppWin = new MainWindow();
    cjbThread *cjbThreadObj = new cjbThread();
    cjbThreadObj->start();
    modbusCommThread *modbusCommThreadObj = new modbusCommThread();
    modbusCommThreadObj->start();
    mainAppWin->show();
    return a.exec();
}
