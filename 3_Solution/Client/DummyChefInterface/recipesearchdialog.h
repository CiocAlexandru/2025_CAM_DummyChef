#ifndef RECIPESEARCHDIALOG_H
#define RECIPESEARCHDIALOG_H

#include <QDialog>
#include <QTcpSocket>
#include <QStringList>

namespace Ui {
class RecipeSearchDialog;
}

class RecipeSearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RecipeSearchDialog(const QString& username, QTcpSocket* socket, QWidget *parent = nullptr);
    ~RecipeSearchDialog();

private slots:
    void onAddKeywordClicked();     // pentru butonul "Adaugă"
    void onSearchClicked();         // pentru butonul "Caută rețete"
    void onSocketReadyRead();       // pentru primirea răspunsului

private:
    Ui::RecipeSearchDialog *ui;
    QString username;
    QTcpSocket* socket;
    QStringList keywords;
};

#endif // RECIPESEARCHDIALOG_H
