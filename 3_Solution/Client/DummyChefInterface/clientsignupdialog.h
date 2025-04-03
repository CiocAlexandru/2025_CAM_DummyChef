#ifndef CLIENTSIGNUPDIALOG_H
#define CLIENTSIGNUPDIALOG_H

#include <QDialog>
#include <QTcpSocket>
#include <QLabel>

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

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::ClientSignUpDialog *ui;
    QTcpSocket *socket;
    QLabel *backgroundLabel;
    void updateBackground();
};

#endif // CLIENTSIGNUPDIALOG_H
