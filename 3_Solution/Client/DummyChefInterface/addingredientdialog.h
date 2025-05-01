#ifndef ADDINGREEDIENTDIALOG_H
#define ADDINGREEDIENTDIALOG_H

#include <QDialog>
#include <QTcpSocket>
#include <QLabel>

namespace Ui {
class AddIngredientDialog;
}

class AddIngredientDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddIngredientDialog(const QString& chefEmail, QTcpSocket* socket, QWidget *parent = nullptr);
    ~AddIngredientDialog();

private slots:
    void on_submitButton_clicked();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::AddIngredientDialog *ui;
    QTcpSocket* socket;
    QString chefEmail;
    QLabel *backgroundLabel;
    void onReadyRead();
    void updateBackground();
};

#endif // ADDINGREEDIENTDIALOG_H
