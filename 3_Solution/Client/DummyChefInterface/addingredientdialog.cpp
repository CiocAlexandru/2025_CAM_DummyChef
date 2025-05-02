#include "addingredientdialog.h"
#include "ui_addingredientdialog.h"
#include <QMessageBox>
#include <QRegularExpression>

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
    ui->phoneSupplierLineEdit->setPlaceholderText("Telefon furnizor");
    ui->mailSupplierLineEdit->setPlaceholderText("Mail furnizor");
    ui->adresaSupplierLineEdit->setPlaceholderText("Adresa furnizor");

    connect(ui->submitButtom, &QPushButton::clicked,
            this, &AddIngredientDialog::on_submitButton_clicked);
    connect(socket, &QTcpSocket::readyRead, this, &AddIngredientDialog::onReadyRead);


}

AddIngredientDialog::~AddIngredientDialog()
{
    delete ui;
}

void AddIngredientDialog::onReadyRead()
{
    QByteArray response = socket->readAll();
    QString reply = QString::fromUtf8(response).trimmed();

    if (reply == "ADD_INGREDIENT_SUCCESS") {
        QMessageBox::information(this, "Succes", "Ingredientul a fost adăugat cu succes în baza de date.");
        accept(); // Închide dialogul
    }
    else if (reply.startsWith("ADD_INGREDIENT_ERROR")) {
        QMessageBox::critical(this, "Eroare", "A apărut o eroare la adăugarea ingredientului:\n" + reply.section(':', 1));
    }
    else if (reply.startsWith("ADD_INGREDIENT_FAILED")) {
        QMessageBox::warning(this, "Format greșit", "Mesajul trimis nu are formatul corect!");
    }
    else {
        QMessageBox::information(this, "Răspuns necunoscut", "Serverul a răspuns:\n" + reply);
    }
}


void AddIngredientDialog::on_submitButton_clicked()
{
    QString name = ui->nameLineEdit->text().trimmed();
    QString price = ui->priceLineEdit->text().trimmed();
    QString stock = ui->stockLineEdit->text().trimmed();
    QString supplier = ui->supplierLineEdit->text().trimmed();
    QString phoneNumber = ui->phoneSupplierLineEdit->text().trimmed();
    QString mail = ui->mailSupplierLineEdit->text().trimmed();
    QString adresa = ui->adresaSupplierLineEdit->text().trimmed();

    // Validare câmpuri
    if (name.isEmpty() || price.isEmpty() || stock.isEmpty() ||
        supplier.isEmpty() || phoneNumber.isEmpty() || mail.isEmpty() || adresa.isEmpty()) {
        QMessageBox::warning(this, "Eroare", "Toate câmpurile trebuie completate.");
        return;
    }

    if (!QRegularExpression(R"(^[^@]+@[^@]+\.[^@]+$)").match(mail).hasMatch()) {
        QMessageBox::warning(this, "Eroare", "Email invalid!");
        return;
    }

    bool ok;
    double priceVal = price.toDouble(&ok);
    if (!ok || priceVal < 0) {
        QMessageBox::warning(this, "Eroare", "Preț invalid.");
        return;
    }

    // Format: ADD_INGREDIENT|email|nume|pret|stoc|furnizor|telefon|email_furnizor|adresa_furnizor
    QString message = QString("ADD_INGREDIENT|%1|%2|%3|%4|%5|%6|%7|%8")
                          .arg(chefEmail)
                          .arg(name.replace("|", " "))
                          .arg(price)
                          .arg(stock)
                          .arg(supplier.replace("|", " "))
                          .arg(phoneNumber.replace("|", " "))
                          .arg(mail.replace("|", " "))
                          .arg(adresa.replace("|", " "));

    socket->write(message.toUtf8());
    QMessageBox::information(this, "Succes", "Ingredient trimis cu succes.");
    accept();
}


void AddIngredientDialog::updateBackground() {
    QPixmap pixmap(":/images/AddIngredient2.jpg");
    backgroundLabel->setPixmap(pixmap);
    backgroundLabel->setGeometry(0, 0, this->width(), this->height());
}

void AddIngredientDialog::resizeEvent(QResizeEvent *event) {
    QDialog::resizeEvent(event);
    updateBackground();
}
