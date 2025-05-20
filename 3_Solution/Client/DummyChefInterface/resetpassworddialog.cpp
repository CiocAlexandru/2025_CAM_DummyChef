#include "resetpassworddialog.h"
#include "ui_resetpassworddialog.h"
#include <QMessageBox>

ResetPasswordDialog::ResetPasswordDialog(const QString &email, QTcpSocket* existingSocket,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResetPasswordDialog),
    socket(existingSocket),
    userEmail(email) // Inițializăm email-ul
{
    ui->setupUi(this);
    setWindowTitle("Resetare Parolă");

    ui->codeLineEdit->setPlaceholderText("Introduceți codul primit");
    ui->newPasswordLineEdit->setPlaceholderText("Introduceți noua parolă");
    ui->newPasswordLineEdit->setEchoMode(QLineEdit::Password);
    ui->confirmPasswordLineEdit->setPlaceholderText("Confirmați noua parolă");
    ui->confirmPasswordLineEdit->setEchoMode(QLineEdit::Password);


    connect(ui->resetButton, &QPushButton::clicked, this, &ResetPasswordDialog::handleResetPassword);
    connect(socket, &QTcpSocket::connected, this, &ResetPasswordDialog::onConnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &ResetPasswordDialog::onError);
    connect(socket, &QTcpSocket::readyRead, this, &ResetPasswordDialog::onReadyRead);
    backgroundLabel = new QLabel(this);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->lower();
    updateBackground();
}

void ResetPasswordDialog::handleResetPassword()
{
    QString code = ui->codeLineEdit->text().trimmed();
    QString newPassword = ui->newPasswordLineEdit->text();
    QString confirmPassword = ui->confirmPasswordLineEdit->text();

    if (code.isEmpty() || newPassword.isEmpty() || confirmPassword.isEmpty()) {
        QMessageBox::warning(this, "Eroare", "Completați toate câmpurile!");
        return;
    }

    if (newPassword != confirmPassword) {
        QMessageBox::warning(this, "Eroare", "Parolele nu coincid!");
        return;
    }
    onConnected();
}

void ResetPasswordDialog::onConnected()
{
    QString message = QString("RESET_PASSWORD %1 %2 %3")
    .arg(userEmail)
        .arg(ui->codeLineEdit->text())
        .arg(ui->newPasswordLineEdit->text());

    socket->write(message.toUtf8());
    socket->flush();

    socket->waitForBytesWritten(3000);

    if (!socket->waitForReadyRead(3000)) {
        QMessageBox::critical(this, "Eroare", "Serverul nu a răspuns în timp util.");
        return;
    }
    onReadyRead();
}

void ResetPasswordDialog::onReadyRead()
{
    QString response = QString::fromUtf8(socket->readAll()).trimmed();

    if (response == "RESET_SUCCESS") {
        QMessageBox::information(this, "Succes", "Parola a fost resetată cu succes!");
        accept();
    } else {
        QMessageBox::warning(this, "Eroare", "Cod invalid sau eroare la resetare!");
    }
}

void ResetPasswordDialog::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError)
    QMessageBox::critical(this, "Eroare", "Eroare conexiune: " + socket->errorString());
}

void ResetPasswordDialog::updateBackground()
{
    QPixmap pixmap(":/images/ResetPassword.jpg");
    if(!pixmap.isNull()) {
        backgroundLabel->setPixmap(pixmap);
        backgroundLabel->setGeometry(0, 0, width(), height());
    }
}

void ResetPasswordDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    setWindowState(windowState() | Qt::WindowFullScreen);
    updateBackground();
}


ResetPasswordDialog::~ResetPasswordDialog()
{
    delete ui;
    socket->deleteLater();
}
