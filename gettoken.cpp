#include "gettoken.h"
#include "ui_gettoken.h"
#include <QUrlQuery>
#include <QMessageBox>

Gettoken::Gettoken(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Gettoken)
{
    ui->setupUi(this);
}

Gettoken::~Gettoken()
{
    delete ui;
}

void Gettoken::on_settoken_clicked()
{
    QUrl u = ui->edit_token->toPlainText();
    if(!u.toString().contains("access_token"))
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Ошибка");
        msgBox.setText("Проверте правильнось ввода!");
        msgBox.exec();
        return;
    }
    u = u.toString().replace("#", "?");
    QUrlQuery query(u);
    QString token = query.queryItemValue("access_token");;
    QMessageBox msgBox;
    emit tokenSet(token);
    msgBox.setWindowTitle("Токен");
    msgBox.setText("Токен успешно получен!");
    msgBox.exec();
    this->close();
}
