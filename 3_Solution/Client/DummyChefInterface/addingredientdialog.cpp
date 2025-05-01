#include "addingredientdialog.h"
#include "ui_addingredientdialog.h"
#include <QMessageBox>

AddIngredientDialog::AddIngredientDialog(const QString& chefEmail, QTcpSocket* socket, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddIngredientDialog),
    socket(socket),
    chefEmail(chefEmail)
{
    ui->setupUi(this);
    setWindowTitle("Adaugă Ingredient");
    backgroundLabel = new QLabel(this);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->lower();


    ui->nameLineEdit->setPlaceholderText("Denumire Ingredient");
    ui->priceLineEdit->setPlaceholderText("Pret");
    ui->stockLineEdit->setPlaceholderText("Cantitate");
    ui->supplierLineEdit->setPlaceholderText("Furnizor");

    connect(ui->submitButtom, &QPushButton::clicked,
            this, &AddIngredientDialog::on_submitButton_clicked);

}

AddIngredientDialog::~AddIngredientDialog()
{
    delete ui;
}

void AddIngredientDialog::on_submitButton_clicked()
{
    QString name = ui->nameLineEdit->text().trimmed();
    QString price = ui->priceLineEdit->text().trimmed();
    QString stock = ui->stockLineEdit->text().trimmed();
    QString supplier = ui->supplierLineEdit->text().trimmed();

    if (name.isEmpty() || price.isEmpty() || stock.isEmpty() || supplier.isEmpty()) {
        QMessageBox::warning(this, "Eroare", "Toate câmpurile trebuie completate.");
        return;
    }

    bool ok;
    double priceVal = price.toDouble(&ok);
    if (!ok || priceVal < 0) {
        QMessageBox::warning(this, "Eroare", "Preț invalid.");
        return;
    }

    QString message = QString("ADD_INGREDIENT|%1|%2|%3|%4|%5")
                          .arg(chefEmail)
                          .arg(name)
                          .arg(price)
                          .arg(stock)
                          .arg(supplier.replace("|", " "));

    socket->write(message.toUtf8());
    QMessageBox::information(this, "Succes", "Ingredient trimis cu succes.");
    accept();
}

void AddIngredientDialog::updateBackground() {
    QPixmap pixmap(":/images/AddIngredient.jpg");
    backgroundLabel->setPixmap(pixmap);
    backgroundLabel->setGeometry(0, 0, this->width(), this->height());
}

void AddIngredientDialog::resizeEvent(QResizeEvent *event) {
    QDialog::resizeEvent(event);
    updateBackground();
}
