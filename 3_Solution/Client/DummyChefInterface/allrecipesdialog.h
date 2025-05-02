#ifndef ALLRECIPESDIALOG_H
#define ALLRECIPESDIALOG_H

#include <QDialog>
#include <QTcpSocket>
#include <QString>
#include <QStringList>
#include <QLabel>

namespace Ui {
class AllRecipesDialog;
}

class AllRecipesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AllRecipesDialog(const QString& username, QTcpSocket* socket, QWidget *parent = nullptr);
    ~AllRecipesDialog();

private slots:
    void onSocketReadyRead();
    void onRecipeSelected();
protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::AllRecipesDialog *ui;
    QString username;
    QTcpSocket* socket;
    QStringList recipeDetails;
    QLabel *backgroundLabel;
    void updateBackground();
};

#endif // ALLRECIPESDIALOG_H
