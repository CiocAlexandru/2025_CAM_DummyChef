#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , socket(new QTcpSocket(this)) // Inițializare socket
{
    ui->setupUi(this);
    this->setStyleSheet("QMainWindow { background-image: url(:/images/DummyChef.jpg); background-position: center; background-repeat: no-repeat; background-size: cover; }");

    // Conectează butoanele din UI la funcții
    connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::connectToServer);
    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::sendMessage);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::readResponse);
    connect(socket, &QTcpSocket::disconnected, this, &MainWindow::handleDisconnect);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Funcția care se conectează la server
void MainWindow::connectToServer()
{
    socket->connectToHost("127.0.0.1", 12345); // IP-ul și portul serverului
    if (socket->waitForConnected(3000)) {
        ui->statusLabel->setText("Conectat la server!");
    } else {
        ui->statusLabel->setText("Eroare la conectare!");
    }
}

// Trimitere mesaj către server
void MainWindow::sendMessage()
{
    QString message = ui->inputField->text(); // Ia textul dintr-un QLineEdit
    if (socket->state() == QTcpSocket::ConnectedState) {
        socket->write(message.toUtf8());
        socket->flush();
    }
}

// Citirea răspunsului de la server
void MainWindow::readResponse()
{
    QByteArray data = socket->readAll();  // Citește tot ce a trimis serverul
    QString mesaj = QString::fromUtf8(data);  // Convertim în string

    qDebug() << "Mesaj primit de la server:" << mesaj; // DEBUG
    ui->outputLabel->setText(mesaj);  // Afișăm mesajul primit
}

// Gestionarea deconectării
void MainWindow::handleDisconnect()
{
    ui->statusLabel->setText("Deconectat de la server.");
}
