#ifndef SHOPPINGLISTDIALOG_H
#define SHOPPINGLISTDIALOG_H

#include <QDialog>
#include <QTcpSocket>

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

private:
    Ui::ShoppingListDialog *ui;
    QTcpSocket* socket;
    QString username;
};

#endif // SHOPPINGLISTDIALOG_H
