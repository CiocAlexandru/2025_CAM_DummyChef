#include "recipesearchdialog.h"
#include "ui_recipesearchdialog.h"
#include <QMessageBox>

RecipeSearchDialog::RecipeSearchDialog(const QString& username, QTcpSocket* socket, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RecipeSearchDialog),
    username(username),
    socket(socket)
{
    ui->setupUi(this);  // Încarcă interfața grafică din .ui
    setWindowTitle("Cauta retete in functie de cuvinte cheie: ");
    backgroundLabel = new QLabel(this);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->lower();

    ui->keywordEdit->setPlaceholderText("Introduceți cuvinte cheie pentru cautare");

    // Conectăm butoanele la funcțiile noastre
    connect(ui->addButton, &QPushButton::clicked, this, &RecipeSearchDialog::onAddKeywordClicked);
    connect(ui->searchButton, &QPushButton::clicked, this, &RecipeSearchDialog::onSearchClicked);
    connect(socket, &QTcpSocket::readyRead, this, &RecipeSearchDialog::onSocketReadyRead);
}

RecipeSearchDialog::~RecipeSearchDialog()
{
    delete ui;
}

void RecipeSearchDialog::onAddKeywordClicked()
{
    QString text = ui->keywordEdit->text().trimmed();
    if (!text.isEmpty()) {
        keywords << text;
        ui->keywordList->addItem(text);
        ui->keywordEdit->clear();
    }
}

void RecipeSearchDialog::onSearchClicked()
{
    if (keywords.isEmpty()) {
        QMessageBox::warning(this, "Atenție", "Adăugați cel puțin un ingredient sau cuvânt!");
        return;
    }

    QString message = "SEARCH_RECIPES " + keywords.join(",");
    socket->write(message.toUtf8());
}

void RecipeSearchDialog::onSocketReadyRead()
{
    QByteArray data = socket->readAll();
    QString response(data);

    if (response.startsWith("RECIPE_RESULTS|")) {
        QString all = response.section("|", 1);
        QStringList entries = all.split("##");

        QStringList finalList;
        for (const QString& entry : entries) {
            QStringList parts = entry.split("|");
            if (parts.size() < 4) continue;

            QString denumire = parts[0];
            QString timp = parts[1];
            QString ingrediente = parts[2];
            QString pasi = parts[3];

            finalList << "🍽 " + denumire +
                             "\n⏱ Timp: " + timp +
                             "\n🧂 Ingrediente:\n - " + ingrediente.replace(";", "\n - ") +
                             "\n👨‍🍳 Pași:\n" + pasi +
                             "\n-------------------";
        }

        QMessageBox::information(this, "Rezultate căutare", "Nu exista astfel de retete!");
        accept();
    }
    else {
        QMessageBox::warning(this, "Eroare", "Eroare la primirea rețetelor.");
    }
}

void RecipeSearchDialog::updateBackground() {
    QPixmap pixmap(":/images/AllRecipes.jpg");
    backgroundLabel->setPixmap(pixmap);
    backgroundLabel->setGeometry(0, 0, this->width(), this->height());
}

void RecipeSearchDialog::resizeEvent(QResizeEvent *event) {
    QDialog::resizeEvent(event);
    updateBackground();
}
