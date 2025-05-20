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
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void handleLogin();
    void onConnected();
    void onError(QAbstractSocket::SocketError socketError);
    void onReadyRead();

private:
    Ui::LoginDialog *ui;
    QLabel *backgroundLabel;
    QTcpSocket *socket;

    void updateBackground();
};

#endif
