#ifndef SHOPPINGLISTDIALOG_H
#define SHOPPINGLISTDIALOG_H

#include <QDialog>
#include <QTcpSocket>
#include <QLabel>

namespace Ui {
class ShoppingListDialog;
}

class ShoppingListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShoppingListDialog(const QString& email, QTcpSocket* socket, QWidget *parent = nullptr);
    ~ShoppingListDialog();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onGenerateListClicked();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError socketError);
    void onPlaceOrderClicked();

private:
    void updateBackground();

    Ui::ShoppingListDialog *ui;
    QTcpSocket* socket;
    QString email;
    QString reteta;
    int idReteta = -1;
    QLabel* backgroundLabel;
};

#endif // SHOPPINGLISTDIALOG_H
