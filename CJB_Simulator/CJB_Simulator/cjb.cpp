#include "cjb.h"
#include "ui_cjb.h"

CJB::CJB(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CJB)
{
    ui->setupUi(this);
}

CJB::~CJB()
{
    delete ui;
}
