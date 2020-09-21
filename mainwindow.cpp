#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "authorization.h"
#include "QUrl"
#include <QUrlQuery>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QDebug>
#include <QTime>
#include <QJsonObject>
#include <QJsonValue>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->label->setText(token);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::url_changed(QUrl u)
{
    if(!u.toString().contains("access_token"))
    {
        return;
    }
    u = u.toString().replace("#", "?");
    QUrlQuery query(u);
    token = query.queryItemValue("access_token");;
    ui->label->setText(token);

}


void MainWindow::on_action_triggered()
{
    Authorization auth(nullptr,this);
    auth.exec();
}

QByteArray MainWindow::GET(QUrl url)
{
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    QNetworkReply* reply = manager->get(QNetworkRequest(url));
    QEventLoop wait;
    connect(manager, SIGNAL(finished (QNetworkReply* )), &wait, SLOT(quit()));
    connect(manager, SIGNAL(finished(QNetworkReply* )), manager, SLOT(deleteLater()));
    QTimer::singleShot(5000, &wait, SLOT(quit()));
    wait.exec();

    QByteArray answer = reply->readAll();
    reply->deleteLater();
    return answer;
}

QByteArray MainWindow::createComment()
{
    QUrl current;
    QUrlQuery query("https://api.vk.com/method/board.createComment?");
    QStringList links = boardLinks();
    query.addQueryItem("v", "5.52");
    query.addQueryItem("access_token", token);
    query.addQueryItem("group_id", links[0]);
    query.addQueryItem("topic_id", links[1]);
    query.addQueryItem("message",ui->message->toPlainText());
    current = query.toString();

    ui->label->setText(current.toString());
    QByteArray answer = GET(current);
    qDebug() << answer;
    return answer;

}

QStringList MainWindow::boardLinks()
{
    QString s = ui->boardLink->toPlainText();
    s = s.replace("https://vk.com/topic-", "");
    QStringList links  = s.split("_");
    return links;
}

bool MainWindow::isValid(QByteArray)
{

}

void MainWindow::on_pushButton_clicked()
{
    for(int i = 0; i < 100; i++)
    {
        createComment();
        QEventLoop wait;
        QTimer::singleShot(3000, &wait, SLOT(quit()));
        wait.exec();
    }
}
