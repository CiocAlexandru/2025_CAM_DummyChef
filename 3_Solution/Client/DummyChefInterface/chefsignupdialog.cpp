#include "chefsignupdialog.h"
#include "ui_chefsignupdialog.h"
#include <QMessageBox>
#include <QRegularExpression>

ChefSignUpDialog::ChefSignUpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChefSignUpDialog),
    socket(new QTcpSocket(this))
{
    ui->setupUi(this);
    setWindowTitle("Înregistrare Bucătar");

    // Configurare câmpuri
    ui->nameLineEdit->setPlaceholderText("Introduceți numele");
    ui->surnameLineEdit->setPlaceholderText("Introduceți prenumele");
    ui->usernameLineEdit->setPlaceholderText("Introduceți numele de utilizator");
    ui->passwordLineEdit->setPlaceholderText("Introduceți parola");
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
    ui->phoneLineEdit->setPlaceholderText("Introduceți numărul de telefon");
    ui->birthDateEdit->setDisplayFormat("dd.MM.yyyy");
    ui->emailLineEdit->setPlaceholderText("Introduceți email");
    ui->experienceYearsLineEdit->setPlaceholderText("Introduceți anii de vechime");
    ui->experienceLinkLineEdit->setPlaceholderText("Introduceți link demonstrativ");

    // Conectare buton sign-up
    connect(ui->signupButton, &QPushButton::clicked, this, &ChefSignUpDialog::handleSignUp);
    connect(socket, &QTcpSocket::connected, this, &ChefSignUpDialog::onConnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &ChefSignUpDialog::onError);
    connect(socket, &QTcpSocket::readyRead, this, &ChefSignUpDialog::onReadyRead);
}

void ChefSignUpDialog::handleSignUp()
{
    QString name = ui->nameLineEdit->text().trimmed();
    QString surname = ui->surnameLineEdit->text().trimmed();
    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();
    QString phone = ui->phoneLineEdit->text().trimmed();
    QString birthDate = ui->birthDateEdit->text();
    QString email = ui->emailLineEdit->text().trimmed();
    QString experienceYears = ui->experienceYearsLineEdit->text().trimmed();
    QString experienceLink = ui->experienceLinkLineEdit->text().trimmed();

    if (name.isEmpty() || surname.isEmpty() || username.isEmpty() || password.isEmpty() ||
        phone.isEmpty() || birthDate.isEmpty() || email.isEmpty() ||
        experienceYears.isEmpty() || experienceLink.isEmpty()) {
        QMessageBox::warning(this, "Eroare", "Completați toate câmpurile!");
        return;
    }

    socket->connectToHost("127.0.0.1", 12345);
}

void ChefSignUpDialog::onConnected()
{
    QString message = QString("SIGNUP_CHEF %1 %2 %3 %4 %5 %6 %7 %8 %9")
    .arg(ui->nameLineEdit->text())
        .arg(ui->surnameLineEdit->text())
        .arg(ui->usernameLineEdit->text())
        .arg(ui->passwordLineEdit->text())
        .arg(ui->phoneLineEdit->text())
        .arg(ui->birthDateEdit->text())
        .arg(ui->emailLineEdit->text())
        .arg(ui->experienceYearsLineEdit->text())
        .arg(ui->experienceLinkLineEdit->text());

    socket->write(message.toUtf8());
}

void ChefSignUpDialog::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError)
    QMessageBox::critical(this, "Eroare", "Eroare conexiune: " + socket->errorString());
}

void ChefSignUpDialog::onReadyRead()
{
    QString response = QString::fromUtf8(socket->readAll()).trimmed();

    if (response == "SIGNUP_CHEF_SUCCESS") {
        QMessageBox::information(this, "Succes", "Cont de bucătar creat cu succes!");
        accept();
    } else {
        QMessageBox::warning(this, "Eroare", "Înregistrare eșuată!");
    }
}

ChefSignUpDialog::~ChefSignUpDialog()
{
    delete ui;
    socket->deleteLater();
}
