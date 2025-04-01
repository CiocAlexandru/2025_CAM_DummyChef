#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);  // Constructor
    ~LoginDialog();

private slots:
    void on_loginButton_clicked();  // Slot pentru butonul de login

private:
    Ui::LoginDialog *ui;
};

#endif
