#include "myordersdialog.h"
#include "ui_myordersdialog.h"
#include <QMessageBox>
#include <QDebug>

MyOrdersDialog::MyOrdersDialog(const QString& email, QTcpSocket* socket, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MyOrdersDialog),
    email(email),
    socket(socket)
{
    ui->setupUi(this);
    setWindowTitle("Comenzile mele");

    requestOrders();  // trimite cererea la server când se deschide
}

MyOrdersDialog::~MyOrdersDialog()
{
    delete ui;
}

void MyOrdersDialog::requestOrders()
{
    if (!socket || !socket->isOpen()) {
        QMessageBox::warning(this, "Eroare", "Conexiunea la server nu este activă.");
        return;
    }

    QString message = "GET_ORDERS|" + email;
    socket->write(message.toUtf8());
    socket->flush();
    socket->waitForReadyRead(3000);  // așteaptă răspuns

    QByteArray response = socket->readAll();
    qDebug() << response;
    parseAndDisplayOrders(QString::fromUtf8(response));
}

void MyOrdersDialog::parseAndDisplayOrders(const QString& response)
{
    ui->ordersListWidget->clear();

    if (response.startsWith("ORDERS|")) {
        QStringList orders = response.mid(7).split("##");

        int index = 1;
        for (const QString& order : orders) {
            QStringList parts = order.split("|");
            if (parts.size() == 2) {
                QString reteta = parts[0];
                QString data = parts[1];
                QString line = QString::number(index) + ". Rețetă comandată: " + reteta + " | Data: " + data;
                ui->ordersListWidget->addItem(line);
                index++;
            }
        }


    } else if (response == "NO_ORDERS") {
        ui->ordersListWidget->addItem("Nu ai comenzi înregistrate.");
    } else {
        QMessageBox::warning(this, "Eroare", "Răspuns necunoscut de la server:\n" + response);
    }
}
