#ifndef FORGOTPASSWORDDIALOG_H
#define FORGOTPASSWORDDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QTcpSocket>

namespace Ui {
class ForgotPasswordDialog;
}

class ForgotPasswordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ForgotPasswordDialog(QWidget *parent = nullptr);
    ~ForgotPasswordDialog();

private slots:
    void handleSendRequest();
    void onConnected();
    void onError(QAbstractSocket::SocketError socketError);
    void onReadyRead();
protected:
    void resizeEvent(QResizeEvent *event) override;
private:
    void updateBackground();
    QLabel *backgroundLabel;
    QTcpSocket *socket;
    Ui::ForgotPasswordDialog *ui;
};

#endif // FORGOTPASSWORDDIALOG_H
