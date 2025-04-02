#include "logindialog.h"
#include <QPixmap>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

LoginDialog::LoginDialog(QWidget *parent) :
    QWidget(parent)  // Schimbat din QDialog în QWidget
{
    // Inițializare elemente UI
    backgroundLabel = new QLabel(this);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->lower();

    // Creare layout și butoane
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel("Dummy Chef");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");

    loginButton = new QPushButton("Login");
    signUpButton = new QPushButton("Sign Up");
    forgotButton = new QPushButton("Forgot Password?");

    // Stilizare butoane
    QString buttonStyle = "QPushButton { padding: 10px; font-size: 16px; }";
    loginButton->setStyleSheet(buttonStyle);
    signUpButton->setStyleSheet(buttonStyle);
    forgotButton->setStyleSheet(buttonStyle);

    // Adăugare elemente în layout
    mainLayout->addSpacing(50);
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(30);
    mainLayout->addWidget(loginButton);
    mainLayout->addWidget(signUpButton);
    mainLayout->addWidget(forgotButton);
    mainLayout->addStretch();

    // Conectare butoane la semnale
    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::handleLogin);
    connect(signUpButton, &QPushButton::clicked, this, &LoginDialog::signUpRequested);
    connect(forgotButton, &QPushButton::clicked, this, &LoginDialog::forgotPasswordRequested);

    setLayout(mainLayout);
    updateBackground();
}

void LoginDialog::updateBackground()
{
    QPixmap pixmap(":/images/LoginnBackground.jpg");
    backgroundLabel->setPixmap(pixmap);
    backgroundLabel->setGeometry(0, 0, this->width(), this->height());
}

void LoginDialog::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateBackground();
}

void LoginDialog::handleLogin()
{
    emit loginRequested();  // Emite semnal în loc să închidă dialogul
}
