#ifndef CLIENTSIGNUPDIALOG_H
#define CLIENTSIGNUPDIALOG_H

#include <QDialog>
#include <QTcpSocket>

namespace Ui {
class ClientSignUpDialog;
}

class ClientSignUpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClientSignUpDialog(QWidget *parent = nullptr);
    ~ClientSignUpDialog();

private slots:
    void handleSignUp();
    void onConnected();
    void onError(QAbstractSocket::SocketError socketError);
    void onReadyRead();

private:
    Ui::ClientSignUpDialog *ui;
    QTcpSocket *socket;
};

#endif // CLIENTSIGNUPDIALOG_H
