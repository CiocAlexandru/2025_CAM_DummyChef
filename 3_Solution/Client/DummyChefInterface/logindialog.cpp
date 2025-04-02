#include "logindialog.h"
#include "ui_logindialog.h"
#include <QMessageBox>
#include <QRegularExpression>

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

    // CONECTĂRI MANUALE:
    // 1. Butonul de login
    connect(ui->loginButton, &QPushButton::clicked,
            this, &LoginDialog::handleLogin);

    // 2. Socket
    connect(socket, &QTcpSocket::connected,
            this, &LoginDialog::onConnected);
    connect(socket, &QTcpSocket::errorOccurred,
            this, &LoginDialog::onError);
    connect(socket, &QTcpSocket::readyRead,
            this, &LoginDialog::onReadyRead);

    // Configurare opțională câmpuri
    ui->emailLineEdit->setPlaceholderText("Introduceți email");
    ui->passwordLineEdit->setPlaceholderText("Introduceți parola");
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
}

void LoginDialog::handleLogin()  // Nume modificat
{
    sendLoginRequest();
}

void LoginDialog::updateBackground()
{
    QPixmap pixmap(":/images/LoginnBackground.jpg");  // Încarcă imaginea
    backgroundLabel->setPixmap(pixmap);
    backgroundLabel->setGeometry(0, 0, this->width(), this->height());  // Ocupă întreaga fereastră
}


void LoginDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    setWindowState(windowState() | Qt::WindowFullScreen);
    updateBackground();  // Redimensionează fundalul
}


void LoginDialog::sendLoginRequest()
{
    QString email = ui->emailLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();

    // Validare
    if(email.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Eroare", "Completați ambele câmpuri!");
        return;
    }

    if(!QRegularExpression(R"(^[^@]+@[^@]+\.[^@]+$)").match(email).hasMatch()) {
        QMessageBox::warning(this, "Eroare", "Email invalid!");
        return;
    }

    socket->connectToHost("127.0.0.1", 12345);
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

    if(response == "LOGIN_SUCCESS") {
        QMessageBox::information(this, "Succes", "Bun venit!");
        accept(); // Închide dialogul cu cod de succes
    } else {
        QMessageBox::warning(this, "Eroare", "Autentificare eșuată!");
    }
}

LoginDialog::~LoginDialog()
{
    delete ui;
    socket->deleteLater();
}
