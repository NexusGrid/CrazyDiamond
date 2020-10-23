#ifndef GETTOKEN_H
#define GETTOKEN_H

#include <QDialog>

namespace Ui {
class Gettoken;
}

class Gettoken : public QDialog
{
    Q_OBJECT

public:
    explicit Gettoken(QWidget *parent = nullptr);
    ~Gettoken();

signals:
   void tokenSet(QString token);

private slots:
    void on_settoken_clicked();

private:
    Ui::Gettoken *ui;
};

#endif // GETTOKEN_H
