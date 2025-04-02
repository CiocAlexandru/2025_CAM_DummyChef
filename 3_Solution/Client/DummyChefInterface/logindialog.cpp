#include "logindialog.h"
#include "ui_logindialog.h"
#include <QPixmap>
#include <QResizeEvent>



LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    setWindowTitle("Login");  // Setează titlul ferestrei

    backgroundLabel = new QLabel(this);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->lower();  // Trimite în spate pentru a nu acoperi UI-ul

    updateBackground();  // Setează imaginea inițial
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
    updateBackground();  // Redimensionează fundalul
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_loginButton_clicked()
{
    accept();  // Închide dialogul cu codul "Accepted"
}
