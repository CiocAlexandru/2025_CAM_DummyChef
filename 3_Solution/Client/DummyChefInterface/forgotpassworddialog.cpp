#include "forgotpassworddialog.h"
#include "ui_forgotpassworddialog.h"
#include <QMessageBox>
#include <QRegularExpression>

#include "resetpassworddialog.h"

ForgotPasswordDialog::ForgotPasswordDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ForgotPasswordDialog),
    socket(new QTcpSocket(this))
{
    ui->setupUi(this);
    setWindowTitle("Recuperare parolă");

    backgroundLabel = new QLabel(this);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->lower();
    ui->emailLineEdit->setPlaceholderText("Introduceți email-ul pentru resetare");

    connect(ui->sendButton, &QPushButton::clicked, this, &ForgotPasswordDialog::handleSendRequest);
    connect(socket, &QTcpSocket::connected, this, &ForgotPasswordDialog::onConnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &ForgotPasswordDialog::onError);
    connect(socket, &QTcpSocket::readyRead, this, &ForgotPasswordDialog::onReadyRead);

    updateBackground();
}


void ForgotPasswordDialog::updateBackground() {
    QPixmap pixmap(":/images/ForgotPassword.jpg");
    backgroundLabel->setPixmap(pixmap);
    backgroundLabel->setGeometry(0, 0, this->width(), this->height());
}

void ForgotPasswordDialog::resizeEvent(QResizeEvent *event) {
    QDialog::resizeEvent(event);
    setWindowState(windowState() | Qt::WindowFullScreen);
    updateBackground();
}

void ForgotPasswordDialog::handleSendRequest()
{
    QString email = ui->emailLineEdit->text().trimmed();

    if (email.isEmpty()) {
        QMessageBox::warning(this, "Eroare", "Introduceți un email valabil!");
        return;
    }
    if (socket->state() == QAbstractSocket::ConnectedState ||
        socket->state() == QAbstractSocket::ConnectingState) {

        socket->abort();
    }
    socket->connectToHost("172.20.10.13", 12345);
}

void ForgotPasswordDialog::onConnected()
{
    QString message = QString("FORGOT_PASSWORD %1").arg(ui->emailLineEdit->text());
    socket->write(message.toUtf8());
}

void ForgotPasswordDialog::onReadyRead()
{
    QString response = QString::fromUtf8(socket->readAll()).trimmed();

    if (response == "EMAIL_FOUND") {
        QMessageBox::information(this, "Succes", "Un cod de resetare a fost trimis la email!");
        ResetPasswordDialog resetDialog(ui->emailLineEdit->text(),socket, this);
        resetDialog.exec();
        this->close();

    } else {
        QMessageBox::warning(this, "Eroare", "Email-ul nu există în baza de date!");
        ui->emailLineEdit->clear();
        ui->emailLineEdit->setFocus();
    }
}

void ForgotPasswordDialog::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError)
    QMessageBox::critical(this, "Eroare", "Eroare conexiune: " + socket->errorString());
}

ForgotPasswordDialog::~ForgotPasswordDialog()
{
    delete ui;
}


