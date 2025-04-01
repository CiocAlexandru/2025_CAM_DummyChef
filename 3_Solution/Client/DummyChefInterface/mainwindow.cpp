#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <QResizeEvent>
#include <QDebug>
#include <QHostAddress> // Pentru adrese IP
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindialog.h"
#include "signupdialog.h"
#include "forgotpassworddialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , socket(new QTcpSocket(this)) // Inițializare socket
{
    ui->setupUi(this);

    // Creăm un QLabel pentru imaginea de fundal
    backgroundLabel = new QLabel(this);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->lower();  // Trimite în spate pentru a nu acoperi UI-ul

    updateBackground();  // Setează imaginea inițial

    // Conectează butoanele din UI la funcții
    connect(ui->signUpButton, &QPushButton::clicked, this, &MainWindow::handleSignUp);
    connect(ui->loginButton, &QPushButton::clicked, this, &MainWindow::handleLogin);
    connect(ui->forgotPasswordButton, &QPushButton::clicked, this, &MainWindow::handleForgotPassword);

    // Conectarea semnalelor și sloturilor pentru socket
    connect(socket, &QTcpSocket::connected, this, &MainWindow::onConnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &MainWindow::onError);


}

MainWindow::~MainWindow()
{
    delete ui;
}

// Funcție pentru a actualiza fundalul
void MainWindow::updateBackground()
{
    QPixmap pixmap(":/images/DummyChefBackground.jpg");  // Încarcă imaginea
    backgroundLabel->setPixmap(pixmap);
    backgroundLabel->setGeometry(0, 0, this->width(), this->height());  // Ocupă întreaga fereastră
}

// Suprascriere resizeEvent pentru a redimensiona imaginea la schimbarea dimensiunii ferestrei
void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    updateBackground();  // Redimensionează fundalul
}

void MainWindow::handleSignUp()
{
    SignUpDialog signUpDialog(this);

    // Conectează semnalul de înregistrare
   /* connect(&signUpDialog, &SignUpDialog::userRegistered, [this](const QString &email, const QString &password) {
        qDebug() << "Utilizator înregistrat:" << email;
        // Trimite datele la server
        socket->connectToHost("127.0.0.1", 12345);
    });*/

    signUpDialog.exec();
}

void MainWindow::handleLogin()
{
    LoginDialog loginDialog(this);
    if (loginDialog.exec() == QDialog::Accepted) {
        qDebug() << "Utilizatorul a dat Login!";
        // Aici poți face conexiunea la server
        socket->connectToHost("127.0.0.1", 12345);
    }
}

void MainWindow::handleForgotPassword()
{
    ForgotPasswordDialog forgotDialog(this);

    // Conectează semnalul de resetare
    connect(&forgotDialog, &ForgotPasswordDialog::passwordResetRequested, [this](const QString &email) {
        qDebug() << "Resetare parolă pentru:" << email;
        // Trimite cererea la server
        socket->write(QString("RESET_PASSWORD %1").arg(email).toUtf8());
    });

    forgotDialog.exec();
}

// Slot care va fi apelat când conexiunea este stabilită
void MainWindow::onConnected()
{
    qDebug() << "Conexiune stabilită cu serverul!";
    // Poți trimite mesaje sau date la server
}

// Slot pentru a gestiona erorile de conexiune
void MainWindow::onError(QTcpSocket::SocketError socketError)
{
    qDebug() << "Eroare la conectare: " << socket->errorString();
}
