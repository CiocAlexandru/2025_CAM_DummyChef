#include "myrecipesdialog.h"
#include "ui_myrecipesdialog.h"
#include <QMessageBox>
#include <QStringList>
#include <QDebug>

MyRecipesDialog::MyRecipesDialog(const QString& chefEmail, QTcpSocket* socket, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MyRecipesDialog),
    socket(socket),
    chefEmail(chefEmail)
{
    ui->setupUi(this);
    setWindowTitle("Rețetele mele");
    backgroundLabel = new QLabel(this);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->lower();

    connect(socket, &QTcpSocket::readyRead, this, &MyRecipesDialog::onReadyRead);

    // Cerem rețetele de la server
    QString request = QString("GET_MY_RECIPES|%1").arg(chefEmail);
    socket->write(request.toUtf8());
}

MyRecipesDialog::~MyRecipesDialog()
{
    delete ui;
}

void MyRecipesDialog::onReadyRead()
{
    QString response = QString::fromUtf8(socket->readAll()).trimmed();

    if (response.startsWith("MY_RECIPES|")) {
        QStringList parts = response.mid(QString("MY_RECIPES|").length()).split("##");

        ui->recipesListWidget->clear();
        for (const QString& recipe : parts) {
            ui->recipesListWidget->addItem(recipe);
        }
    } else {
        QMessageBox::warning(this, "Eroare", "Nu s-au putut încărca rețetele.");
    }
}

void MyRecipesDialog::updateBackground() {
    QPixmap pixmap(":/images/ChefMainWindow.jpg");
    backgroundLabel->setPixmap(pixmap);
    backgroundLabel->setGeometry(0, 0, this->width(), this->height());
}

void MyRecipesDialog::resizeEvent(QResizeEvent *event) {
    QDialog::resizeEvent(event);
    updateBackground();
}
