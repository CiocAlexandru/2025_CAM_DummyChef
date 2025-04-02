#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <QResizeEvent>
#include <QDebug>
#include <QHostAddress>
#include "logindialog.h"
#include "signupdialog.h"
#include "forgotpassworddialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , socket(new QTcpSocket(this))
{
    ui->setupUi(this);

    // Inițializare stackedWidget
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    // Creare și adăugare pagini
    loginDialog = new LoginDialog();
    signUpDialog = new SignUpDialog();
    forgotPasswordDialog = new ForgotPasswordDialog();

    stackedWidget->addWidget(loginDialog);
    stackedWidget->addWidget(signUpDialog);
    stackedWidget->addWidget(forgotPasswordDialog);

    // Conectare semnale
    connect(loginDialog, &LoginDialog::loginRequested, this, &MainWindow::handleLogin);
    connect(loginDialog, &LoginDialog::signUpRequested, this, [this]() {
        stackedWidget->setCurrentWidget(signUpDialog);
    });
    connect(loginDialog, &LoginDialog::forgotPasswordRequested, this, [this]() {
        stackedWidget->setCurrentWidget(forgotPasswordDialog);
    });

    // Conectare semnale pentru socket
    connect(socket, &QTcpSocket::connected, this, &MainWindow::onConnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &MainWindow::onError);

    // Setează pagina inițială
    stackedWidget->setCurrentWidget(loginDialog);

    // Fundal
    backgroundLabel = new QLabel(this);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->lower();
    updateBackground();
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
    stackedWidget->setCurrentWidget(signUpDialog);
}

void MainWindow::handleLogin()
{
    qDebug() << "Utilizatorul a dat Login!";
    socket->connectToHost("127.0.0.1", 12345);
    // După login reușit, poți schimba la pagina principală a aplicației
    // stackedWidget->setCurrentWidget(mainAppPage);
}

void MainWindow::handleForgotPassword()
{
    stackedWidget->setCurrentWidget(forgotPasswordDialog);
}

void MainWindow::onConnected()
{
    qDebug() << "Conexiune stabilită cu serverul!";
}

void MainWindow::onError(QTcpSocket::SocketError socketError)
{
    qDebug() << "Eroare la conectare: " << socket->errorString();
}
