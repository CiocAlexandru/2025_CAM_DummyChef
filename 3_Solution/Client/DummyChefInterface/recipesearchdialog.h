#ifndef RECIPESEARCHDIALOG_H
#define RECIPESEARCHDIALOG_H

#include <QDialog>
#include <QTcpSocket>
#include <QStringList>
#include <QLabel>

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
    void onAddKeywordClicked();
    void onSearchClicked();
    void onSocketReadyRead();
protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::RecipeSearchDialog *ui;
    QString username;
    QTcpSocket* socket;
    QStringList keywords;
    QLabel *backgroundLabel;
    void updateBackground();
};

#endif // RECIPESEARCHDIALOG_H
