#include "forgotpassworddialog.h"
#include "ui_forgotpassworddialog.h"
#include <QMessageBox>
#include <QRegularExpression>

ForgotPasswordDialog::ForgotPasswordDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ForgotPasswordDialog)
{
    ui->setupUi(this);
    setWindowTitle("Recuperare parolă");

    // Conectează modificarea emailului la validare
    //connect(ui->emailLineEdit, &QLineEdit::textChanged, this, &ForgotPasswordDialog::validateEmail);

    // Dezactivează butonul inițial
    //ui->resetButton->setEnabled(false);
}

ForgotPasswordDialog::~ForgotPasswordDialog()
{
    delete ui;
}

void ForgotPasswordDialog::validateEmail()
{
    //const QString email = ui->emailLineEdit->text();
    //bool isValid = QRegularExpression("^[^@]+@[^@]+\\.[^@]+$").match(email).hasMatch();

    //ui->resetButton->setEnabled(isValid);
   // ui->emailLineEdit->setStyleSheet(isValid ? "" : "border: 1px solid red;");
}

void ForgotPasswordDialog::on_resetButton_clicked()
{
   // const QString email = ui->emailLineEdit->text();

    // Emite semnalul cu emailul
    //emit passwordResetRequested(email);

    // Afișează confirmare
    QMessageBox::information(this, "Link trimis", "Un link de resetare a fost trimis la adresa de email.");

    accept(); // Închide dialogul
}
