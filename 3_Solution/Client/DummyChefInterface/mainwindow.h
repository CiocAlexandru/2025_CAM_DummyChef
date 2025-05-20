#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event) override;


private slots:
    void handleSignUp();
    void handleLogin();
    void handleForgotPassword();
    void onConnected();
    void onError(QTcpSocket::SocketError socketError);


private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
    QLabel *backgroundLabel;

    void updateBackground();
};

#endif // MAINWINDOW_H
