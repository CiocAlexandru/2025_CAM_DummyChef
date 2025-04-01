#include "signupdialog.h"
#include "ui_signupdialog.h"
#include <QMessageBox>
#include <QRegularExpression>

SignUpDialog::SignUpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SignUpDialog)
{
    ui->setupUi(this);
    setWindowTitle("Înregistrare");

    // Conectează schimbările din câmpuri la validare
    //connect(ui->emailLineEdit, &QLineEdit::textChanged, this, &SignUpDialog::validateInputs);
    //connect(ui->passwordLineEdit, &QLineEdit::textChanged, this, &SignUpDialog::validateInputs);
    //connect(ui->confirmPasswordLineEdit, &QLineEdit::textChanged, this, &SignUpDialog::validateInputs);

    // Dezactivează butonul inițial
    //ui->signUpButton->setEnabled(false);
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
