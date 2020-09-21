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
#include <QJsonDocument>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
    QMessageBox msgBox;
    msgBox.setWindowTitle("Токен");
    msgBox.setText("Токен успешно получен! Можете закрыть окно авторизации");
    msgBox.exec();

}


void MainWindow::on_action_triggered()
{
    Authorization auth(nullptr,this);
    auth.exec();
}

QJsonDocument MainWindow::GET(QUrl url)
{
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    QNetworkReply* reply = manager->get(QNetworkRequest(url));
    QEventLoop wait;
    connect(manager, SIGNAL(finished (QNetworkReply* )), &wait, SLOT(quit()));
    connect(manager, SIGNAL(finished(QNetworkReply* )), manager, SLOT(deleteLater()));
    QTimer::singleShot(5000, &wait, SLOT(quit()));
    wait.exec();

    QJsonDocument answer = QJsonDocument::fromJson(reply->readAll());
    reply->deleteLater();
    return answer;
}

bool MainWindow::createComment()
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

    QJsonDocument answer = GET(current);
    qDebug() << answer;
    return isValid(answer);


}

QStringList MainWindow::boardLinks()
{
    QString s = ui->boardLink->toPlainText();
    s = s.replace("https://vk.com/topic-", "");
    QStringList links  = s.split("_");
    return links;
}

bool MainWindow::isValid(QJsonDocument json)
{
    QJsonObject root = json.object();
    if(!root.value("error").isUndefined()) return false;
    if(!root.value("response").isUndefined()) return true;

}

void MainWindow::displayRemainingTime()
{
    QTime displayTime = ui->timeEdit->time();
    QTime currentTime = QTime::currentTime();
    int secsTo = currentTime.secsTo(displayTime);
    QTime temp(0,0,0);
    temp = temp.addSecs(secsTo);
    qDebug() << temp.toString();
    ui->timeDisplays->setText(temp.toString());

}

void MainWindow::on_pushButton_clicked()
{
    do
    {
        displayRemainingTime();
        QEventLoop wait;
        QTimer::singleShot(500, &wait, SLOT(quit()));
        wait.exec();

    }while(ui->timeEdit->time().toString() != QTime::currentTime().toString());

    if(ui->timeEdit->time().toString() >= QTime::currentTime().toString()){
        while(!createComment())
        {
            QEventLoop wait;
            QTimer::singleShot(3000, &wait, SLOT(quit()));
            wait.exec();
        }
    }
}
