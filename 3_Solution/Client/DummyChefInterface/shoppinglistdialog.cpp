#include "shoppinglistdialog.h"
#include "ui_shoppinglistdialog.h"
#include <QMessageBox>

ShoppingListDialog::ShoppingListDialog(const QString& username, const QString& reteta, QTcpSocket* socket, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShoppingListDialog),
    socket(socket),
    username(username),
    reteta(reteta)
{
    ui->setupUi(this);
    setWindowTitle("Lista de cumpărături");
    backgroundLabel = new QLabel(this);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->lower();
    updateBackground();
    connect(socket, &QTcpSocket::readyRead, this, &ShoppingListDialog::onReadyRead);
    connect(socket, &QTcpSocket::errorOccurred, this, &ShoppingListDialog::onError);

    connect(ui->placeOrderButton, &QPushButton::clicked, this, &ShoppingListDialog::onPlaceOrderClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &ShoppingListDialog::reject);

    // Trimitere comandă spre server
    QString message = "GENERARE_LISTA " + username + " " + reteta + "\n";
    socket->write(message.toUtf8());

    // Inițial, dezactivez butonul de comandă până la răspuns
    ui->placeOrderButton->setEnabled(false);
}

void ShoppingListDialog::onReadyRead()
{
    QString response = QString::fromUtf8(socket->readAll());

    if (response.trimmed() == "NU_EXISTA_PRODUSE") {
        QMessageBox::warning(this, "Eroare", "Nu există produse disponibile conform preferințelor dumneavoastră.");
        ui->shoppingListTextEdit->setPlainText("Nu există produse disponibile conform preferințelor dumneavoastră.");
        ui->placeOrderButton->setEnabled(false);
    } else {
        ui->shoppingListTextEdit->setPlainText(response);
        ui->placeOrderButton->setEnabled(true);
    }
}

void ShoppingListDialog::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    QMessageBox::critical(this, "Eroare conexiune", socket->errorString());
}

void ShoppingListDialog::onPlaceOrderClicked()
{
    QString message = "PLASEAZA_COMANDA " + username + " " + reteta + "\n";
    socket->write(message.toUtf8());

    QMessageBox::information(this, "Comandă trimisă", "Comanda a fost plasată cu succes!");
    accept();
}

ShoppingListDialog::~ShoppingListDialog()
{
    delete ui;
}

void ShoppingListDialog::updateBackground()
{
    QPixmap pixmap(":/images/ShoppingList.jpg");
    if(!pixmap.isNull()) {
        backgroundLabel->setPixmap(pixmap);
        backgroundLabel->setGeometry(0, 0, width(), height());
    }
}

void ShoppingListDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event); // Apelare funcție de bază
    setWindowState(windowState() | Qt::WindowFullScreen);
    updateBackground(); // Actualizare dimensiuni fundal
}
