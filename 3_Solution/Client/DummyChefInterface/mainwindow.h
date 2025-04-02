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
    void resizeEvent(QResizeEvent *event) override;  // Suprascriem resizeEvent pentru fundal


private slots:
    void handleSignUp();      // Funcție pentru Sign Up
    void handleLogin();       // Funcție pentru Login
    void handleForgotPassword(); // Funcție pentru Forgot Password
    void onConnected();// Slot pentru a gestiona conexiunea
    void onError(QTcpSocket::SocketError socketError);// Slot pentru a gestiona erorile


private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
    QLabel *backgroundLabel;  // Adăugăm QLabel pentru fundal

    void updateBackground();  // Funcție pentru actualizarea fundalului
};

#endif // MAINWINDOW_H
