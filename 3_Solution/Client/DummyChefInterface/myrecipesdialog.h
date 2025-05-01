#ifndef MYRECIPESDIALOG_H
#define MYRECIPESDIALOG_H

#include <QDialog>
#include <QTcpSocket>
#include <QLabel>

namespace Ui {
class MyRecipesDialog;
}

class MyRecipesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MyRecipesDialog(const QString& chefEmail, QTcpSocket* socket, QWidget *parent = nullptr);
    ~MyRecipesDialog();

private slots:
    void onReadyRead();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::MyRecipesDialog *ui;
    QTcpSocket* socket;
    QString chefEmail;
    QLabel *backgroundLabel;

    void updateBackground();
};

#endif // MYRECIPESDIALOG_H
