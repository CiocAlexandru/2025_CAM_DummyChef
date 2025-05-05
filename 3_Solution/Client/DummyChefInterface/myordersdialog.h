#ifndef MYORDERSDIALOG_H
#define MYORDERSDIALOG_H

#include <QDialog>
#include <QTcpSocket>

namespace Ui {
class MyOrdersDialog;
}

class MyOrdersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MyOrdersDialog(const QString& email, QTcpSocket* socket, QWidget *parent = nullptr);
    ~MyOrdersDialog();

private:
    void requestOrders();
    void parseAndDisplayOrders(const QString& response);

    Ui::MyOrdersDialog *ui;
    QString email;
    QTcpSocket* socket;
};

#endif // MYORDERSDIALOG_H
