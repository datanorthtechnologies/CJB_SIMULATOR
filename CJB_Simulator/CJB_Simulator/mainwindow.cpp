#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "externs.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    modbusCommObj = new modbusComm(this);
    cjbObj = new CJB(ui->frmMain);
    cjbObj->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}
