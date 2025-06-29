#include "signupdialog.h"
#include "ui_signupdialog.h"
#include <QPixmap>

SignUpDialog::SignUpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SignUpDialog)
{
    ui->setupUi(this);
    setWindowTitle("Selectare tip cont");

    connect(ui->clientButton, &QPushButton::clicked,
            this, &SignUpDialog::on_clientButton_clicked);
    connect(ui->bucatarButton, &QPushButton::clicked,
            this, &SignUpDialog::on_bucatarButton_clicked);

    backgroundLabel = new QLabel(this);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->lower();
    updateBackground();
}

void SignUpDialog::on_clientButton_clicked()
{
    ClientSignUpDialog *clientDialog = new ClientSignUpDialog(this);
    clientDialog->exec();
    delete clientDialog;
}


void SignUpDialog::on_bucatarButton_clicked()
{
    ChefSignUpDialog *chefDialog = new ChefSignUpDialog(this);
    chefDialog->exec();
    delete chefDialog;
 }

void SignUpDialog::updateBackground()
{
    QPixmap pixmap(":/images/SignUp2.jpg");
    if(!pixmap.isNull()) {
        backgroundLabel->setPixmap(pixmap);
        backgroundLabel->setGeometry(0, 0, width(), height());
    }
}

void SignUpDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    setWindowState(windowState() | Qt::WindowFullScreen);
    updateBackground();
}

SignUpDialog::~SignUpDialog()
{
    delete ui;
}


