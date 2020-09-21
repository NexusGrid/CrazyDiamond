#include "authorization.h"
#include "ui_authorization.h"
#include "mainwindow.h"

Authorization::Authorization(QWidget *parent, MainWindow* m) :
    QDialog(parent),
    ui(new Ui::Authorization)
{
    ui->setupUi(this);
    connect(ui->WebView, SIGNAL(urlChanged(QUrl)), m,SLOT(url_changed(QUrl)));
    ui->WebView->load(QUrl("https://oauth.vk.com/authorize?client_id=7600925&display=page&redirect_uri=&scope=groups&response_type=token&v=5.124&state=123456"));

}


Authorization::~Authorization()
{
    delete ui;
}
