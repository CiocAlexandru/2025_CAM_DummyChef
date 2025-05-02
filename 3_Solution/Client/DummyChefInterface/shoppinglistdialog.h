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
    explicit ShoppingListDialog(const QString& username, QTcpSocket* socket, QWidget *parent = nullptr);
    ~ShoppingListDialog();

private slots:
    void onReadyRead();
    void onError(QAbstractSocket::SocketError socketError);
    void onPlaceOrderClicked();
    void onGenerateListClicked(); // Nou slot

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::ShoppingListDialog *ui;
    QTcpSocket* socket;
    QString username;
    QString reteta;
    QLabel *backgroundLabel;

    void updateBackground();
};

#endif // SHOPPINGLISTDIALOG_H
