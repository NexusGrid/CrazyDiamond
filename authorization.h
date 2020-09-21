#ifndef AUTHORIZATION_H
#define AUTHORIZATION_H

#include <QDialog>
#include <mainwindow.h>

namespace Ui {
class Authorization;
}

class Authorization : public QDialog
{
    Q_OBJECT

public:
    explicit Authorization(QWidget *parent = nullptr, MainWindow *m = nullptr);
    ~Authorization();

private:
    Ui::Authorization *ui;
};

#endif // AUTHORIZATION_H
