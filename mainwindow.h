#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUrl>
#include <QJsonDocument>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void statusCheck(QJsonDocument);
    void url_changed(QUrl);

private slots:
    void on_action_triggered();

    void on_pushButton_clicked();


private:
    Ui::MainWindow *ui;
    QString token;
    QJsonDocument GET(QUrl);
    bool createComment();
    QStringList boardLinks();
    bool isValid(QJsonDocument);
    void displayRemainingTime();
};
#endif // MAINWINDOW_H
