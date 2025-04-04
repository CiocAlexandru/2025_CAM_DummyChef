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
    explicit ResetPasswordDialog(const QString &email, QWidget *parent = nullptr);
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
    QString userEmail;
};

#endif // RESETPASSWORDDIALOG_H
