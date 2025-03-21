#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <QResizeEvent>
#include <QDebug>
#include <QHostAddress> // Pentru adrese IP

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
    QPixmap pixmap(":/images/DummyChef.jpg");  // Încarcă imaginea
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
    qDebug() << "Sign Up button clicked!";
    // Conectează-te la server
    socket->connectToHost("127.0.0.1", 12345); // IP-ul și portul serverului
}

void MainWindow::handleLogin()
{
    qDebug() << "Login button clicked!";
    // Conectează-te la server
    socket->connectToHost("127.0.0.1", 12345); // IP-ul și portul serverului
}

void MainWindow::handleForgotPassword()
{
    qDebug() << "Forgot Password button clicked!";
    // Poți implementa această funcționalitate aici
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
