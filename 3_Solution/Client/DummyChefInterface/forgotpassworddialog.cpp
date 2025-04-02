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
    backgroundLabel = new QLabel(this);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->lower();
    // Conectează modificarea emailului la validare
    //connect(ui->emailLineEdit, &QLineEdit::textChanged, this, &ForgotPasswordDialog::validateEmail);

    // Dezactivează butonul inițial
    //ui->resetButton->setEnabled(false);
    updateBackground();
}

void ForgotPasswordDialog::updateBackground() {
    QPixmap pixmap(":/images/SignUp.jpg");  // Încarcă imaginea din resurse
    backgroundLabel->setPixmap(pixmap);
    backgroundLabel->setGeometry(0, 0, this->width(), this->height());  // Acoperă întreaga fereastră
}

void ForgotPasswordDialog::resizeEvent(QResizeEvent *event) {
    QDialog::resizeEvent(event);
    setWindowState(windowState() | Qt::WindowFullScreen);
    updateBackground();  // Actualizează dimensiunea fundalului la redimensionare
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
