#ifndef ADDRECIPEDIALOG_H
#define ADDRECIPEDIALOG_H

#include <QDialog>
#include <QTcpSocket>
#include <QLabel>

namespace Ui {
class AddRecipeDialog;
}

class AddRecipeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddRecipeDialog(const QString& chefEmail, QTcpSocket* socket, QWidget *parent = nullptr);
    ~AddRecipeDialog();

private slots:
    void on_addIngredientButton_clicked();
    void on_submitButton_clicked();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::AddRecipeDialog *ui;
    QTcpSocket* socket;
    QString chefEmail;
    QLabel *backgroundLabel;

    void updateBackground();
};

#endif // ADDRECIPEDIALOG_H
