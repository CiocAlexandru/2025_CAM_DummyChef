#ifndef CHEFSIGNUPDIALOG_H
#define CHEFSIGNUPDIALOG_H

#include <QDialog>
#include <QTcpSocket>

namespace Ui {
class ChefSignUpDialog;
}

class ChefSignUpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChefSignUpDialog(QWidget *parent = nullptr);
    ~ChefSignUpDialog();

private slots:
    void handleSignUp();
    void onConnected();
    void onError(QAbstractSocket::SocketError socketError);
    void onReadyRead();

private:
    Ui::ChefSignUpDialog *ui;
    QTcpSocket *socket;
};

#endif // CHEFSIGNUPDIALOG_H
