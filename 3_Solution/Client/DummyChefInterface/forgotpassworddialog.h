#ifndef FORGOTPASSWORDDIALOG_H
#define FORGOTPASSWORDDIALOG_H

#include <QDialog>

namespace Ui {
class ForgotPasswordDialog;
}

class ForgotPasswordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ForgotPasswordDialog(QWidget *parent = nullptr);
    ~ForgotPasswordDialog();

signals:
    void passwordResetRequested(const QString &email); // Semnal pentru resetare parolă

private slots:
    void on_resetButton_clicked(); // Slot pentru butonul de resetare
    void validateEmail(); // Validare email în timp real

private:
    Ui::ForgotPasswordDialog *ui;
};

#endif // FORGOTPASSWORDDIALOG_H
