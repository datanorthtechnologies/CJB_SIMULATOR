#ifndef CJB_H
#define CJB_H

#include <QWidget>

namespace Ui {
class CJB;
}

class CJB : public QWidget
{
    Q_OBJECT

public:
    explicit CJB(QWidget *parent = 0);
    ~CJB();

private:
    Ui::CJB *ui;
};

#endif // CJB_H
