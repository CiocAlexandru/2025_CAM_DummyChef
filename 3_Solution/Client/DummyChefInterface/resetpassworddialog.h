#ifndef RESETPASSWORDDIALOG_H
#define RESETPASSWORDDIALOG_H

#include <QDialog>
#include <QTcpSocket>
#include <QLabel>
namespace Ui {
class ResetPasswordDialog;
}
class ResetPasswordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResetPasswordDialog(QWidget *parent = nullptr);
    ~ResetPasswordDialog();

private slots:
    void handleResetPassword();
    void onConnected();
    void onError(QAbstractSocket::SocketError socketError);
    void onReadyRead();
protected:
    void resizeEvent(QResizeEvent *event) override;
private:
    Ui::ResetPasswordDialog *ui;
    QTcpSocket *socket;
    QLabel *backgroundLabel;
    void updateBackground();
};

#endif // RESETPASSWORDDIALOG_H
