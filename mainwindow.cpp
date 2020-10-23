#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "authorization.h"
#include "gettoken.h"
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
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pushButton->setDisabled(true);
    ui->pushButton_Stop->setDisabled(true);
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
    msgBox.setText("Токен успешно получен!");
    msgBox.exec();
    ui->pushButton->setEnabled(true);
    ui->status->setText("");
    emit closeChildWindow();

}

void MainWindow::setToken(QString tempToken)
{
    token = tempToken;
    ui->pushButton->setEnabled(true);
    ui->status->setText("");
}


void MainWindow::on_action_triggered()
{
    Authorization auth(nullptr,this);
    connect(this, SIGNAL(closeChildWindow()), &auth,SLOT(closeWindow()));
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
    return answer;
}

void MainWindow::createComment()
{
    QUrl current;
    QUrlQuery query("https://api.vk.com/method/board.createComment?");
    QStringList links = boardLinks();
    query.addQueryItem("v", "5.52");
    query.addQueryItem("access_token", token);
    query.addQueryItem("group_id", links[0]);
    query.addQueryItem("topic_id", links[1]);
    current = query.toString();

    QString temp = QUrl::toPercentEncoding(ui->message->toPlainText());
    current = query.toString();
    current = current.toString() + "&message=" + temp;

    QJsonDocument answer = GET(current);
    statusCheck(answer);
    QString strJson(answer.toJson(QJsonDocument::Compact));
    qDebug() << current.toString();
    qDebug() << strJson;
    return;


}

QStringList MainWindow::boardLinks()
{
    QString s = ui->boardLink->toPlainText();
    s = s.replace("https://vk.com/topic-", "");
    QStringList links  = s.split("_");
    return links;
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

bool MainWindow::isBoardClosed()
{
    QUrl current;
    QUrlQuery query("https://api.vk.com/method/board.getTopics?");
    QStringList links = boardLinks();
    query.addQueryItem("v", "5.52");
    query.addQueryItem("access_token", token);
    query.addQueryItem("group_id", links[0]);
    query.addQueryItem("topic_ids", links[1]);
    query.addQueryItem("extended", "0");
    query.addQueryItem("preview_length", "0");
    current = query.toString();
    QJsonDocument answer = GET(current);
    QJsonObject jsonAnswer = answer.object();
    QJsonObject response = jsonAnswer.value("response").toObject();
    QJsonArray items = response.value("items").toArray();
    int isClosed;

    foreach (const QJsonValue & value, items) {
        QJsonObject obj = value.toObject();
        if(obj.contains("is_closed"))
        {
            isClosed = obj.value("is_closed").toInt();
        }
    }

    QString strJson(answer.toJson(QJsonDocument::Compact));

    qDebug() << strJson;

    if(isClosed == 0) return false;
    if(isClosed == 1) return true;
}

void MainWindow::on_pushButton_clicked()
{
    ui->pushButton->setDisabled(true);
    ui->pushButton_Stop->setEnabled(true);
    QStringList links = boardLinks();
    if(links[0] == ""){ui->pushButton->setEnabled(true);ui->pushButton_Stop->setDisabled(true);QMessageBox msgBox; msgBox.critical(0,"Ошибка","Неверная ссылка на обсуждение!");return;}
    if(links[1] == ""){ui->pushButton->setEnabled(true);ui->pushButton_Stop->setDisabled(true);QMessageBox msgBox; msgBox.critical(0,"Ошибка","Неверная ссылка на обсуждение!");return;}
    if(ui->message->toPlainText() == ""){ui->pushButton->setEnabled(true);ui->pushButton_Stop->setDisabled(true);QMessageBox msgBox; msgBox.critical(0,"Ошибка","Отсутствует текст комментария!"); return;}
    do
    {   
        if(!ui->pushButton_Stop->isEnabled())
        {
            ui->pushButton->setEnabled(true);
            ui->timeDisplays->setText("00:00:00");
            return;
        }
        displayRemainingTime();
        QEventLoop wait;
        QTimer::singleShot(500, &wait, SLOT(quit()));
        wait.exec();

    }while(ui->timeEdit->time().toString() != QTime::currentTime().toString());

    if(ui->timeEdit->time().toString() >= QTime::currentTime().toString()){
        int i = 0;
        while(isBoardClosed())
        {
            QEventLoop wait;
            QTimer::singleShot(334, &wait, SLOT(quit()));
            wait.exec();
            qDebug() << i++;

        }
    }
    createComment();
    ui->timeDisplays->setText("00:00:00");
    ui->pushButton->setEnabled(true);
    ui->pushButton_Stop->setDisabled(true);
}

void MainWindow::on_pushButton_Stop_clicked()
{
    ui->pushButton_Stop->setDisabled(true);
}


void MainWindow::on_action_2_triggered()
{
    Gettoken gettoken(this);
    connect(&gettoken, SIGNAL(tokenSet(QString)), this, SLOT(setToken(QString)));
    gettoken.setWindowTitle("Ручной ввод токена");
    gettoken.exec();

}
