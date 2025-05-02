#include "allrecipesdialog.h"
#include "ui_allrecipesdialog.h"
#include <QMessageBox>

AllRecipesDialog::AllRecipesDialog(const QString& username, QTcpSocket* socket, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AllRecipesDialog),
    username(username),
    socket(socket)
{
    ui->setupUi(this);
    setWindowTitle("Vezi toate retetele: ");
    backgroundLabel = new QLabel(this);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->lower();

    connect(socket, &QTcpSocket::readyRead, this, &AllRecipesDialog::onSocketReadyRead);
    connect(ui->recipeListWidget, &QListWidget::itemClicked, this, &AllRecipesDialog::onRecipeSelected);

    socket->write("GET_ALL_RECIPES\n");
}

AllRecipesDialog::~AllRecipesDialog()
{
    delete ui;
}

void AllRecipesDialog::onSocketReadyRead()
{
    QByteArray data = socket->readAll();
    QString response(data);

    if (response.startsWith("ALL_RECIPES|")) {
        QString all = response.section("|", 1);
        QStringList entries = all.split("##");

        recipeDetails = entries;
        for (const QString& entry : entries) {
            QString title = entry.section("|", 0, 0);
            ui->recipeListWidget->addItem("🍲 " + title);
        }
    } else {
        QMessageBox::warning(this, "Eroare", "Nu s-au putut încărca rețetele.");
    }
}

void AllRecipesDialog::onRecipeSelected()
{
    int index = ui->recipeListWidget->currentRow();
    if (index < 0 || index >= recipeDetails.size()) return;

    QStringList parts = recipeDetails[index].split("|");
    if (parts.size() < 4) return;

    QString denumire = parts[0];
    QString timp = parts[1];
    QString ingrediente = parts[2];
    QString pasi = parts[3];

    QString detalii = "🍽 " + denumire +
                      "\n⏱ Timp: " + timp +
                      "\n🧂 Ingrediente:\n - " + ingrediente.replace(";", "\n - ") +
                      "\n👨‍🍳 Pași:\n" + pasi;

    QMessageBox::information(this, "Detalii rețetă", detalii);
}

void AllRecipesDialog::updateBackground() {
    QPixmap pixmap(":/images/AllRecipes.jpg");
    backgroundLabel->setPixmap(pixmap);
    backgroundLabel->setGeometry(0, 0, this->width(), this->height());
}

void AllRecipesDialog::resizeEvent(QResizeEvent *event) {
    QDialog::resizeEvent(event);
    updateBackground();
}
