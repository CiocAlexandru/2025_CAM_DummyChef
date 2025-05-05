#include "shoppinglistdialog.h"
#include "ui_shoppinglistdialog.h"
#include <QMessageBox>

ShoppingListDialog::ShoppingListDialog(const QString& email, QTcpSocket* socket, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShoppingListDialog),
    socket(socket),
    email(email)
{
    ui->setupUi(this);
    setWindowTitle("Lista de cumpărături");

    backgroundLabel = new QLabel(this);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->lower();
    updateBackground();

    ui->recipeLineEdit->setPlaceholderText("Introduceți rețeta care vă interesează");

    connect(socket, &QTcpSocket::readyRead, this, &ShoppingListDialog::onReadyRead);
    connect(socket, &QTcpSocket::errorOccurred, this, &ShoppingListDialog::onError);
    connect(ui->placeOrderButton, &QPushButton::clicked, this, &ShoppingListDialog::onPlaceOrderClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &ShoppingListDialog::reject);
    connect(ui->generateListButton, &QPushButton::clicked, this, &ShoppingListDialog::onGenerateListClicked);

    ui->placeOrderButton->setEnabled(false);
}

ShoppingListDialog::~ShoppingListDialog()
{
    delete ui;
}

void ShoppingListDialog::onGenerateListClicked()
{
    QString retetaIntroduse = ui->recipeLineEdit->text().trimmed();
    if (retetaIntroduse.isEmpty()) {
        QMessageBox::warning(this, "Input lipsă", "Introduceți numele rețetei.");
        return;
    }

    reteta = retetaIntroduse;
    QString message = "GENERARE_LISTA " + email + " " + reteta + "\n";
    socket->write(message.toUtf8());

    ui->placeOrderButton->setEnabled(false);
}

void ShoppingListDialog::onReadyRead()
{
    QString response = QString::fromUtf8(socket->readAll()).trimmed();

    if (response == "NU_EXISTA_PRODUSE") {
        QMessageBox::warning(this, "Eroare", "Nu există produse disponibile conform preferințelor dumneavoastră.");
        ui->shoppingListTextEdit->setPlainText(response);
        ui->placeOrderButton->setEnabled(false);
        return;
    }

    if (response.startsWith("ID_RETETA|")) {
        QStringList parts = response.split("|");
        if (parts.size() >= 3) {
            idReteta = parts[1].toInt();
            QString content = parts.mid(2).join("|");
            ui->shoppingListTextEdit->setPlainText(content);
            ui->placeOrderButton->setEnabled(true);
        } else {
            ui->shoppingListTextEdit->setPlainText("Eroare la interpretarea răspunsului de la server.");
            ui->placeOrderButton->setEnabled(false);
        }
    } else {
        ui->shoppingListTextEdit->setPlainText(response);
        ui->placeOrderButton->setEnabled(false);
    }
}

void ShoppingListDialog::onPlaceOrderClicked()
{
    if (idReteta == -1) {
        QMessageBox::warning(this, "Eroare", "ID-ul rețetei nu este valid.");
        return;
    }

    QString message = "PLACE_ORDER|" + email + "|" + QString::number(idReteta) + "\n";  // SCHIMBAT
    socket->write(message.toUtf8());

    QMessageBox::information(this, "Comandă trimisă", "Comanda a fost plasată cu succes!");
    accept();

}

void ShoppingListDialog::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    QMessageBox::critical(this, "Eroare conexiune", socket->errorString());
}

void ShoppingListDialog::updateBackground()
{
    QPixmap pixmap(":/images/ShoppingList.jpg");
    if (!pixmap.isNull()) {
        backgroundLabel->setPixmap(pixmap);
        backgroundLabel->setGeometry(0, 0, width(), height());
    }
}

void ShoppingListDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    updateBackground();
}
