#include "clientsignupdialog.h"
#include "ui_clientsignupdialog.h"
#include "clientpreferencesdialog.h"
#include <QMessageBox>
#include <QRegularExpression>

ClientSignUpDialog::ClientSignUpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClientSignUpDialog),
    socket(new QTcpSocket(this))
{
    ui->setupUi(this);
    setWindowTitle("Înregistrare Client");
    backgroundLabel = new QLabel(this);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->lower();

    ui->nameLineEdit->setPlaceholderText("Introduceți numele");
    ui->surnameLineEdit->setPlaceholderText("Introduceți prenumele");
    ui->usernameLineEdit->setPlaceholderText("Introduceți numele de utilizator");
    ui->passwordLineEdit->setPlaceholderText("Introduceți parola");
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
    ui->confirmPasswordLineEdit->setPlaceholderText("Confirmați parola");
    ui->confirmPasswordLineEdit->setEchoMode(QLineEdit::Password);
    ui->phoneLineEdit->setPlaceholderText("Introduceți numărul de telefon");
    ui->birthDateEdit->setDisplayFormat("dd.MM.yyyy");
    ui->emailLineEdit->setPlaceholderText("Introduceți email");
    ui->adressLineEdit->setPlaceholderText("Introduceți adresa de livrare");

    connect(ui->signupButton, &QPushButton::clicked, this, &ClientSignUpDialog::handleSignUp);
    connect(socket, &QTcpSocket::connected, this, &ClientSignUpDialog::onConnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &ClientSignUpDialog::onError);
    connect(socket, &QTcpSocket::readyRead, this, &ClientSignUpDialog::onReadyRead);

    updateBackground();
}

void ClientSignUpDialog::handleSignUp()
{
    QString name = ui->nameLineEdit->text().trimmed();
    QString surname = ui->surnameLineEdit->text().trimmed();
    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();
    QString confirmPassword = ui->confirmPasswordLineEdit->text();
    QString phone = ui->phoneLineEdit->text().trimmed();
    QString birthDate = ui->birthDateEdit->text();
    QString email = ui->emailLineEdit->text().trimmed();
    QString address = ui->adressLineEdit->text().trimmed();

    if (name.isEmpty() || surname.isEmpty() || username.isEmpty() || password.isEmpty() || confirmPassword.isEmpty() || phone.isEmpty() || birthDate.isEmpty() || email.isEmpty() || address.isEmpty()) {
        QMessageBox::warning(this, "Eroare", "Completați toate câmpurile!");
        return;
    }
    if (password != confirmPassword) {
        QMessageBox::warning(this, "Eroare", "Parolele nu coincid!");
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

void ClientSignUpDialog::onConnected()
{
    QString message = QString("SIGNUP CLIENT %1 %2 %3 %4 %5 %6 %7 %8")
    .arg(ui->nameLineEdit->text())
        .arg(ui->surnameLineEdit->text())
        .arg(ui->usernameLineEdit->text())
        .arg(ui->passwordLineEdit->text())
        .arg(ui->phoneLineEdit->text())
        .arg(ui->birthDateEdit->text())
        .arg(ui->emailLineEdit->text())
        .arg(ui->adressLineEdit->text());

    socket->write(message.toUtf8());
}

void ClientSignUpDialog::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError)
    QMessageBox::critical(this, "Eroare", "Eroare conexiune: " + socket->errorString());
}

void ClientSignUpDialog::onReadyRead()
{
    QString response = QString::fromUtf8(socket->readAll()).trimmed();

    if (response == "SIGNUP_CLIENT_SUCCESS") {
        QMessageBox::information(this, "Succes", "Cont creat cu succes!");
        accept();

        QString username = ui->usernameLineEdit->text().trimmed();
        ClientPreferencesDialog prefDialog(username, socket, this);
        prefDialog.exec();
    } else {
        QMessageBox::warning(this, "Eroare", "Înregistrare eșuată!");

        ui->nameLineEdit->clear();
        ui->surnameLineEdit->clear();
        ui->usernameLineEdit->clear();
        ui->passwordLineEdit->clear();
        ui->confirmPasswordLineEdit->clear();
        ui->phoneLineEdit->clear();
        ui->birthDateEdit->setDate(QDate::currentDate());
        ui->emailLineEdit->clear();
        ui->adressLineEdit->clear();

        ui->nameLineEdit->setFocus();
    }
}


void ClientSignUpDialog::updateBackground() {
    QPixmap pixmap(":/images/LoginClient.jpg");
    backgroundLabel->setPixmap(pixmap);
    backgroundLabel->setGeometry(0, 0, this->width(), this->height());
}

void ClientSignUpDialog::resizeEvent(QResizeEvent *event) {
    QDialog::resizeEvent(event);
    setWindowState(windowState() | Qt::WindowFullScreen);
    updateBackground();
}

ClientSignUpDialog::~ClientSignUpDialog()
{
    delete ui;
    socket->deleteLater();
}
