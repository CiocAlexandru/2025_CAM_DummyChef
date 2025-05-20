#include "addrecipedialog.h"
#include "ui_addrecipedialog.h"
#include <QMessageBox>

AddRecipeDialog::AddRecipeDialog(const QString& chefEmail, QTcpSocket* socket, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddRecipeDialog),
    socket(socket),
    chefEmail(chefEmail)
{
    ui->setupUi(this);
    setWindowTitle("Adaugă Rețetă");
    backgroundLabel = new QLabel(this);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->lower();

    ui->ingredientsTable->setColumnCount(2);
    ui->ingredientsTable->setHorizontalHeaderLabels({"Ingredient", "Cantitate"});
    ui->ingredientsTable->horizontalHeader()->setStretchLastSection(true);

    ui->prepTimeEdit->setDisplayFormat("hh:mm");
    ui->recipeNameLineEdit->setPlaceholderText("Denumirea retetei");
    ui->stepsTextEdit->setPlaceholderText("Pasii de preparare");

     disconnect(ui->addIngredientButton, nullptr, nullptr, nullptr);
    connect(ui->addIngredientButton, &QPushButton::clicked,
            this, &AddRecipeDialog::on_addIngredientButton_clicked);

    connect(ui->submitButton, &QPushButton::clicked,
            this, &AddRecipeDialog::on_submitButton_clicked);

    connect(socket, &QTcpSocket::readyRead, this, &AddRecipeDialog::onSocketReadyRead);

}

AddRecipeDialog::~AddRecipeDialog()
{
    delete ui;
}

void AddRecipeDialog::onSocketReadyRead() {
    disconnect(socket, &QTcpSocket::readyRead, this, &AddRecipeDialog::onSocketReadyRead);

    QByteArray response = socket->readAll();
    QString message = QString::fromUtf8(response).trimmed();

    if (message == "ADD_RECIPE_SUCCESS") {
        QMessageBox::information(this, "Succes", "Rețeta a fost salvată cu succes!");
        accept();
    }
    else if (message.startsWith("ADD_RECIPE_ERROR")) {
        QString errorDetail = message.section(':', 1).trimmed();
        QMessageBox::critical(this, "Eroare la salvare",
                              errorDetail.isEmpty() ? "A apărut o eroare necunoscută." : errorDetail);
    }
    else if (message.startsWith("ADD_RECIPE_FAILED")) {
        QString failReason = message.section(':', 1).trimmed();
        QMessageBox::warning(this, "Salvare eșuată",
                             failReason.isEmpty() ? "Rețeta nu a putut fi salvată." : failReason);
    }
    else {
        QMessageBox::information(this, "Răspuns server", message);
    }
}


void AddRecipeDialog::on_addIngredientButton_clicked()
{
    int row = ui->ingredientsTable->rowCount();
    ui->ingredientsTable->insertRow(row);
}

void AddRecipeDialog::on_submitButton_clicked()
{
    if (recipeAlreadySent)
    {
        return;
    }
    recipeAlreadySent = true;
    QString name = ui->recipeNameLineEdit->text().trimmed();
    QString steps = ui->stepsTextEdit->toPlainText().trimmed();
    QString prepTime = ui->prepTimeEdit->time().toString("hh:mm");

    if (name.isEmpty() || steps.isEmpty() || ui->ingredientsTable->rowCount() == 0) {
        QMessageBox::warning(this, "Eroare", "Toate câmpurile trebuie completate!");
        return;
    }

    QStringList ingredientList;
    for (int i = 0; i < ui->ingredientsTable->rowCount(); ++i) {
        QTableWidgetItem* item1 = ui->ingredientsTable->item(i, 0);
        QTableWidgetItem* item2 = ui->ingredientsTable->item(i, 1);
        if (!item1 || !item2 || item1->text().isEmpty() || item2->text().isEmpty()) {
            QMessageBox::warning(this, "Eroare", "Toate ingredientele trebuie completate.");
            return;
        }
        QString ing = item1->text().replace("|", " ");
        QString qty = item2->text().replace("|", " ");
        ingredientList.append(ing + ":" + qty);
    }

    QString ingredientsStr = ingredientList.join(";");

    QString message = QString("ADD_RECIPE|%1|%2|%3|%4|%5")
                          .arg(chefEmail)
                          .arg(name)
                          .arg(prepTime)
                          .arg(ingredientsStr)
                          .arg(steps.replace("|", " "));

    socket->write(message.toUtf8());
    socket->flush();
}

void AddRecipeDialog::updateBackground() {
    QPixmap pixmap(":/images/AddRecipe.jpg");
    backgroundLabel->setPixmap(pixmap);
    backgroundLabel->setGeometry(0, 0, this->width(), this->height());
}

void AddRecipeDialog::resizeEvent(QResizeEvent *event) {
    QDialog::resizeEvent(event);
    updateBackground();
}


