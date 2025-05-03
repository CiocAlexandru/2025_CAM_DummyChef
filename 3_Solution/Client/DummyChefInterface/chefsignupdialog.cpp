#include "chefsignupdialog.h"
#include "ui_chefsignupdialog.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QRegularExpression>

ChefSignUpDialog::ChefSignUpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChefSignUpDialog),
    socket(new QTcpSocket(this))
{
    ui->setupUi(this);
    setWindowTitle("Înregistrare Bucătar");
    backgroundLabel = new QLabel(this);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->lower();

    // Configurare câmpuri
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
    ui->experienceYearsLineEdit->setPlaceholderText("Introduceți anii de vechime");
    ui->experienceLinkLineEdit->setPlaceholderText("Introduceți link demonstrativ");

    // Conectare buton sign-up
    connect(ui->signupButton, &QPushButton::clicked, this, &ChefSignUpDialog::handleSignUp);
    connect(socket, &QTcpSocket::connected, this, &ChefSignUpDialog::onConnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &ChefSignUpDialog::onError);
    connect(socket, &QTcpSocket::readyRead, this, &ChefSignUpDialog::onReadyRead);

    updateBackground();
}

void ChefSignUpDialog::handleSignUp()
{
    QString name = ui->nameLineEdit->text().trimmed();
    QString surname = ui->surnameLineEdit->text().trimmed();
    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();
    QString confirmPassword = ui->confirmPasswordLineEdit->text();
    QString phone = ui->phoneLineEdit->text().trimmed();
    QString birthDate = ui->birthDateEdit->text();
    QString email = ui->emailLineEdit->text().trimmed();
    QString experienceYears = ui->experienceYearsLineEdit->text().trimmed();
    QString experienceLink = ui->experienceLinkLineEdit->text().trimmed();

    if (name.isEmpty() || surname.isEmpty() || username.isEmpty() || password.isEmpty() ||confirmPassword.isEmpty() ||
        phone.isEmpty() || birthDate.isEmpty() || email.isEmpty() ||
        experienceYears.isEmpty() || experienceLink.isEmpty()) {
        QMessageBox::warning(this, "Eroare", "Completați toate câmpurile!");
        return;
    }

    if (password != confirmPassword) {
        QMessageBox::warning(this, "Eroare", "Parolele nu coincid!");
        return;
    }

    // Dacă e deja conectat sau în proces de conectare, deconectează-l
    if (socket->state() == QAbstractSocket::ConnectedState ||
        socket->state() == QAbstractSocket::ConnectingState) {

        socket->abort();  // Închide imediat conexiunea curentă
    }

    socket->connectToHost("127.0.0.1", 12345);
}

void ChefSignUpDialog::onConnected()
{
    QString message = QString("SIGNUP CHEF %1 %2 %3 %4 %5 %6 %7 %8 %9")
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

        if (socket && socket->isOpen()) {
            socket->abort();  // Închide conexiunea imediat
        }

        this->close();  // Închide dialogul

        MainWindow* mainWin = new MainWindow();
        mainWin->show();  // Reafișează fereastra principală
        return;
    }

    else if(response == "Link_demonstrativ_refuzat")
    {
        QMessageBox::information(this, "Eroare", "Link demonstrativ fals!");
        ui->nameLineEdit->clear();
        ui->surnameLineEdit->clear();
        ui->usernameLineEdit->clear();
        ui->passwordLineEdit->clear();
        ui->confirmPasswordLineEdit->clear();
        ui->phoneLineEdit->clear();
        ui->birthDateEdit->setDate(QDate::currentDate());  // Reset la data curentă
        ui->emailLineEdit->clear();
        ui->experienceYearsLineEdit->clear();
        ui->experienceLinkLineEdit->clear();
        ui->nameLineEdit->setFocus();  // Focus pe primul câmp
    }
    else {
        QMessageBox::warning(this, "Eroare", "Înregistrare eșuată!");
        // Golește toate câmpurile
        ui->nameLineEdit->clear();
        ui->surnameLineEdit->clear();
        ui->usernameLineEdit->clear();
        ui->passwordLineEdit->clear();
        ui->confirmPasswordLineEdit->clear();
        ui->phoneLineEdit->clear();
        ui->birthDateEdit->setDate(QDate::currentDate());  // Reset la data curentă
        ui->emailLineEdit->clear();
        ui->experienceYearsLineEdit->clear();
        ui->experienceLinkLineEdit->clear();
        ui->nameLineEdit->setFocus();  // Focus pe primul câmp
    }
}

void ChefSignUpDialog::updateBackground() {
    QPixmap pixmap(":/images/LoginBucatar.jpg");  // Încarcă imaginea din resurse
    backgroundLabel->setPixmap(pixmap);
    backgroundLabel->setGeometry(0, 0, this->width(), this->height());  // Acoperă întreaga fereastră
}

void ChefSignUpDialog::resizeEvent(QResizeEvent *event) {
    QDialog::resizeEvent(event);
    setWindowState(windowState() | Qt::WindowFullScreen);
    updateBackground();  // Actualizează dimensiunea fundalului la redimensionare
}

ChefSignUpDialog::~ChefSignUpDialog()
{
    delete ui;
    socket->deleteLater();
}
