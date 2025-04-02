#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui {class LoginDialog;}
QT_END_NAMESPACE

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);  // Constructor
    ~LoginDialog();

protected:
    void resizeEvent(QResizeEvent *event) override;    // Suprascriem resizeEvent pentru fundal

private slots:
    void handleLogin();    // Slot pentru butonul de login
    void onConnected();  // Slot pentru conexiune reușită
    void onError(QAbstractSocket::SocketError socketError);  // Slot pentru erori
    void onReadyRead();

private:
    Ui::LoginDialog *ui;
    QLabel *backgroundLabel;  // Adăugăm QLabel pentru fundal
    QTcpSocket *socket;

    void updateBackground();
    void sendLoginRequest();

};

#endif
