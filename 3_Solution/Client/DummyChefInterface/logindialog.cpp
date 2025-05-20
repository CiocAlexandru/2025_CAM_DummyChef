#include "logindialog.h"
#include "ui_logindialog.h"
#include "clientmainwindow.h"
#include "chefmainwindow.h"
#include <QMessageBox>
#include <QRegularExpression>
#include <QDebug>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog),
    socket(new QTcpSocket(this))
{
    ui->setupUi(this);
    setWindowTitle("Login");

    backgroundLabel = new QLabel(this);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->lower();

    updateBackground();

    connect(ui->loginButton, &QPushButton::clicked,
            this, &LoginDialog::handleLogin);

    connect(socket, &QTcpSocket::connected,
            this, &LoginDialog::onConnected);
    connect(socket, &QTcpSocket::errorOccurred,
            this, &LoginDialog::onError);
    connect(socket, &QTcpSocket::readyRead,
            this, &LoginDialog::onReadyRead);

    ui->usernameLineEdit->setPlaceholderText("Introduceți username");
    ui->emailLineEdit->setPlaceholderText("Introduceți email");
    ui->passwordLineEdit->setPlaceholderText("Introduceți parola");
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
}

void LoginDialog::handleLogin()
{
    QString username = ui->usernameLineEdit->text().trimmed();
    QString email = ui->emailLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();

    if (username.isEmpty() || email.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Eroare", "Completați toate câmpurile!");
        return;
    }

    if (!QRegularExpression(R"(^[^@]+@[^@]+\.[^@]+$)").match(email).hasMatch()) {
        QMessageBox::warning(this, "Eroare", "Email invalid!");
        return;
    }

    if (socket->state() == QAbstractSocket::ConnectedState ||
        socket->state() == QAbstractSocket::ConnectingState) {

        socket->abort();
    }

    socket->connectToHost("172.20.10.13", 12345);
}

void LoginDialog::updateBackground()
{
    QPixmap pixmap(":/images/LoginBuna.jpg");
    backgroundLabel->setPixmap(pixmap);
    backgroundLabel->setGeometry(0, 0, this->width(), this->height());
}


void LoginDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    setWindowState(windowState() | Qt::WindowFullScreen);
    updateBackground();
}


void LoginDialog::onConnected()
{
    QString message = QString("LOGIN %1 %2")
    .arg(ui->emailLineEdit->text())
        .arg(ui->passwordLineEdit->text());
    socket->write(message.toUtf8());
}

void LoginDialog::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError)
    QMessageBox::critical(this, "Eroare",
                          "Eroare conexiune: " + socket->errorString());
}

void LoginDialog::onReadyRead()
{
    QString response = QString::fromUtf8(socket->readAll()).trimmed();
    qDebug() << "Răspuns de la server:" << response;
    QString username = ui->usernameLineEdit->text().trimmed();
    QString email = ui->emailLineEdit->text().trimmed();


   if (response == "LOGIN_SUCCESS_CLIENT") {
        QMessageBox::information(this, "Succes", "Autentificare reușită ca CLIENT!");

        socket->setParent(nullptr);
        ClientMainWindow *clientWindow = new ClientMainWindow(username,email, socket);
        socket->setParent(clientWindow);
        clientWindow->show();
        socket->readAll();
        this->accept();
    } else if (response == "LOGIN_SUCCESS_CHEF") {
        QMessageBox::information(this, "Succes", "Autentificare reușită ca BUCĂTAR!");

        socket->setParent(nullptr);
        ChefMainWindow *chefWindow = new ChefMainWindow(email, socket);
        socket->setParent(chefWindow);
        chefWindow->show();
        socket->readAll();
        this->accept();
    }
    else {
        QMessageBox::warning(this, "Eroare", "Email sau parolă incorecte!");
        ui->usernameLineEdit->clear();
        ui->emailLineEdit->clear();
        ui->passwordLineEdit->clear();
    }
}



LoginDialog::~LoginDialog()
{
    delete ui;
}
