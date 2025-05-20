#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <QResizeEvent>
#include <QDebug>
#include <QHostAddress> // Pentru adrese IP
#include "logindialog.h"
#include "signupdialog.h"
#include "forgotpassworddialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , socket(new QTcpSocket(this))
{
    ui->setupUi(this);


    backgroundLabel = new QLabel(this);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->lower();

    updateBackground();

    connect(ui->signUpButton, &QPushButton::clicked, this, &MainWindow::handleSignUp);
    connect(ui->loginButton, &QPushButton::clicked, this, &MainWindow::handleLogin);
    connect(ui->forgotPasswordButton, &QPushButton::clicked, this, &MainWindow::handleForgotPassword);

    connect(socket, &QTcpSocket::connected, this, &MainWindow::onConnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &MainWindow::onError);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateBackground()
{
    QPixmap pixmap(":/images/DummyChefBackground.jpg");
    backgroundLabel->setPixmap(pixmap);
    backgroundLabel->setGeometry(0, 0, this->width(), this->height());
}


void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    updateBackground();
}

void MainWindow::handleSignUp()
{
    this->hide();

    SignUpDialog signUpDialog(this);

    if (signUpDialog.exec() == QDialog::Accepted)
    {
        qDebug() << "Utilizatorul s-a înregistrat cu succes!";
    }
    else {
        this->show();
    }
}

void MainWindow::handleLogin()
{
    this->hide();
    LoginDialog loginDialog(this);
    if (loginDialog.exec() == QDialog::Accepted)
    {
        qDebug() << "Utilizatorul a dat Login!";
    }
    else
    {
        this->show();
    }
}

void MainWindow::handleForgotPassword()
{
    this->hide();

    ForgotPasswordDialog forgotDialog(this);

    int result = forgotDialog.exec();
    qDebug() << "ForgotPasswordDialog exec() returned: " << result;

    if (result == QDialog::Accepted) {
        qDebug() << "Resetare parolă confirmată!";
    } else {
        qDebug() << "Resetare parolă anulată!";
        this->show();
    }
}



void MainWindow::onConnected()
{
    qDebug() << "Conexiune stabilită cu serverul!";
}

void MainWindow::onError(QTcpSocket::SocketError socketError)
{
    qDebug() << "Eroare la conectare: " << socket->errorString();
}
