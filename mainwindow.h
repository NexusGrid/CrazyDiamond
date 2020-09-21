#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUrl>

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
    void url_changed(QUrl);

private slots:
    void on_action_triggered();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QString token;
    QByteArray GET(QUrl);
    void createComment();
    QStringList boardLinks();
};
#endif // MAINWINDOW_H
