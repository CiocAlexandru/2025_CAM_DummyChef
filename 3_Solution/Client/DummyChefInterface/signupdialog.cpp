#include "signupdialog.h"
#include "ui_signupdialog.h"
#include <QMessageBox>
#include <QRegularExpression>
#include <QPixmap>
#include <QResizeEvent>

SignUpDialog::SignUpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SignUpDialog)
{
    ui->setupUi(this);
    setWindowTitle("Înregistrare");

    backgroundLabel = new QLabel(this);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->lower();  // Trimite în spate pentru a nu acoperi UI-ul

    updateBackground();  // Setează imaginea inițial
    // Conectează schimbările din câmpuri la validare
    //connect(ui->emailLineEdit, &QLineEdit::textChanged, this, &SignUpDialog::validateInputs);
    //connect(ui->passwordLineEdit, &QLineEdit::textChanged, this, &SignUpDialog::validateInputs);
    //connect(ui->confirmPasswordLineEdit, &QLineEdit::textChanged, this, &SignUpDialog::validateInputs);

    // Dezactivează butonul inițial
    //ui->signUpButton->setEnabled(false);
}

void SignUpDialog::updateBackground()
{
    QPixmap pixmap(":/images/SignUp.jpg");  // Încarcă imaginea
    backgroundLabel->setPixmap(pixmap);
    backgroundLabel->setGeometry(0, 0, this->width(), this->height());  // Ocupă întreaga fereastră
}

void SignUpDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    updateBackground();  // Redimensionează fundalul
}


SignUpDialog::~SignUpDialog()
{
    delete ui;
}

/*void SignUpDialog::validateInputs()
{
    const QString email = ui->emailLineEdit->text();
    const QString password = ui->passwordLineEdit->text();
    const QString confirmPassword = ui->confirmPasswordLineEdit->text();

    // Validare email
    bool isEmailValid = QRegularExpression("^[^@]+@[^@]+\\.[^@]+$").match(email).hasMatch();

    // Validare parolă (minim 6 caractere)
    bool isPasswordValid = (password.length() >= 6);

    // Parolele trebuie să coincidă
    bool passwordsMatch = (password == confirmPassword);

    // Activează butonul doar dacă toate condițiile sunt îndeplinite
    ui->signUpButton->setEnabled(isEmailValid && isPasswordValid && passwordsMatch);

    // Feedback vizual
    ui->emailLineEdit->setStyleSheet(isEmailValid ? "" : "border: 1px solid red;");
    ui->passwordLineEdit->setStyleSheet(isPasswordValid ? "" : "border: 1px solid red;");
    ui->confirmPasswordLineEdit->setStyleSheet(passwordsMatch ? "" : "border: 1px solid red;");
}*/

/*void SignUpDialog::on_signUpButton_clicked()
{
    const QString email = ui->emailLineEdit->text();
    const QString password = ui->passwordLineEdit->text();

    // Emite semnalul cu datele utilizatorului
    emit userRegistered(email, password);

    // Închide dialogul cu cod de succes
    accept();
}*/
