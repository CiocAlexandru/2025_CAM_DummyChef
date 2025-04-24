#include "shoppinglistdialog.h"
#include "ui_shoppinglistdialog.h"
#include <QMessageBox>

ShoppingListDialog::ShoppingListDialog(const QString& username, QTcpSocket* socket, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShoppingListDialog),
    socket(socket),
    username(username)
{
    ui->setupUi(this);
    setWindowTitle("Lista de cumpărături");

    connect(socket, &QTcpSocket::readyRead, this, &ShoppingListDialog::onReadyRead);
    connect(socket, &QTcpSocket::errorOccurred, this, &ShoppingListDialog::onError);

    // Trimite cererea către server
    QString message = "GENERARE_LISTA " + username;
    socket->write(message.toUtf8());
}

void ShoppingListDialog::onReadyRead()
{
    QString response = QString::fromUtf8(socket->readAll());
    ui->shoppingListTextEdit->setPlainText(response);
}

void ShoppingListDialog::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    QMessageBox::critical(this, "Eroare conexiune", socket->errorString());
}

ShoppingListDialog::~ShoppingListDialog()
{
    delete ui;
}
