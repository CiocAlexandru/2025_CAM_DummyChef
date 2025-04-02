#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

class LoginDialog : public QWidget  // Schimbat din QDialog în QWidget
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

signals:  // Adăugăm semnale pentru comunicarea cu MainWindow
    void loginRequested();
    void signUpRequested();
    void forgotPasswordRequested();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void handleLogin();  // Redenumit din on_loginButton_clicked pentru consistență

private:
    QLabel *backgroundLabel;
    QPushButton *loginButton;
    QPushButton *signUpButton;
    QPushButton *forgotButton;
    QVBoxLayout *layout;

    void updateBackground();
    void setupUI();  // Funcție separată pentru inițializarea UI
};

#endif // LOGINDIALOG_H
