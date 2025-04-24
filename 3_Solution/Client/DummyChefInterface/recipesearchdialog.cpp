#include "recipesearchdialog.h"
#include "ui_recipesearchdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QListWidget>
#include <QLabel>
#include <QMessageBox>

RecipeSearchDialog::RecipeSearchDialog(const QString& username, QTcpSocket* socket, QWidget *parent)
    : QDialog(parent), username(username), socket(socket), ui(nullptr)
{
    setWindowTitle("Căutare rețete");
    resize(400, 300);

    // Layout principal
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Label introductiv
    QLabel *label = new QLabel("Introduceți ingrediente sau cuvinte cheie:");
    mainLayout->addWidget(label);

    // Layout pentru adăugare cuvânt
    QHBoxLayout *inputLayout = new QHBoxLayout();
    QLineEdit *keywordEdit = new QLineEdit(this);
    QPushButton *addButton = new QPushButton("Adaugă", this);
    inputLayout->addWidget(keywordEdit);
    inputLayout->addWidget(addButton);
    mainLayout->addLayout(inputLayout);

    // Listă cuvinte adăugate
    QListWidget *keywordList = new QListWidget(this);
    mainLayout->addWidget(keywordList);

    // Buton căutare
    QPushButton *searchButton = new QPushButton("Caută rețete", this);
    mainLayout->addWidget(searchButton);

    // Conectări
    connect(addButton, &QPushButton::clicked, this, [=]() {
        QString text = keywordEdit->text().trimmed();
        if (!text.isEmpty()) {
            keywords << text;
            keywordList->addItem(text);
            keywordEdit->clear();
        }
    });

    connect(searchButton, &QPushButton::clicked, this, [=]() {
        if (keywords.isEmpty()) {
            QMessageBox::warning(this, "Atenție", "Adăugați cel puțin un ingredient sau cuvânt!");
            return;
        }

        QString message = "SEARCH_RECIPES " + keywords.join(",");
        socket->write(message.toUtf8());

        QMessageBox::information(this, "Căutare trimisă", "Cererea a fost trimisă serverului.");
        // TODO: Așteaptă răspuns și afișează rețetele primite
    });
}


RecipeSearchDialog::~RecipeSearchDialog() {}
