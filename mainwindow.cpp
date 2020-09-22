#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "authorization.h"
#include "QUrl"
#include <QUrlQuery>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QTime>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QMessageBox>
#include <QJsonArray>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pushButton->setDisabled(true);
    ui->status->setText("Требуется авторизация");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::statusCheck(QJsonDocument json)
{
     QJsonObject root = json.object();
     if(!root.value("error").isUndefined())
     {
        QJsonValue error = root.value("error");
        QJsonObject temp = error.toObject();
        QJsonValue jsonValue = temp.value("error_code");
        int value = jsonValue.toInt();

        switch (value)
        {
        case 100:
            ui->status->setText("Ошибка!\nОдин или несколько аргументов \nневерны или отсутствуют!");
            break;
        case 14:
            ui->status->setText("Ошибка!\nТребуется ввод капчи!");
            break;
        case 6:
            ui->status->setText("Ошибка!\nСлишком много запросов в секунду!");
            break;
        case 15:
            ui->status->setText("Ошибка!\nОбсуждение закрыто!");
            break;
        }

     }
     if(!root.value("response").isUndefined())
     {
         QJsonObject::Iterator response = root.find("response");
         int value = response.value().toInt();

         ui->status->setText("Успешно!\nКомментарий оставлен под id " + QString::number(value) + "!");
     }


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
    ui->pushButton->setEnabled(true);
    ui->status->setText("");

}


void MainWindow::on_action_triggered()
{
    Authorization auth(nullptr,this);
    auth.setWindowTitle("Авторизация");
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
    statusCheck(answer);
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
    ui->timeDisplays->setText(temp.toString());

}

void MainWindow::on_pushButton_clicked()
{
    QStringList links = boardLinks();
    if(links[0] == ""){QMessageBox msgBox; msgBox.critical(0,"Ошибка","Неверная ссылка на обсуждение!");return;}
    if(links[1] == ""){QMessageBox msgBox; msgBox.critical(0,"Ошибка","Неверная ссылка на обсуждение!");return;}
    if(ui->message->toPlainText() == ""){QMessageBox msgBox; msgBox.critical(0,"Ошибка","Отсутствует текст комментария!"); return;}
    ui->pushButton->setDisabled(true);
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
    ui->timeDisplays->setText("00:00:00");
    ui->pushButton->setEnabled(true);
}
