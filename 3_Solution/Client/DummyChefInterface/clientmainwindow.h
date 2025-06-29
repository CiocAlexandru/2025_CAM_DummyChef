#ifndef CLIENTMAINWINDOW_H
#define CLIENTMAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QLabel>
#include "clientpreferencesdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ClientMainWindow; }
QT_END_NAMESPACE

class ClientMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ClientMainWindow(const QString& username,const QString& email, QTcpSocket* socket, QWidget *parent = nullptr);
    ~ClientMainWindow();

private slots:
    void openSearchRecipes();
    void openShoppingList();
    void openModifyPrefrences();
    void openAllRecipes();
    void handleLogout();
    void openOrderHistory();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::ClientMainWindow *ui;
    QString username;
    QString email;
    QTcpSocket *socket;
    QLabel *backgroundLabel;
    void updateBackground();
};

#endif // CLIENTMAINWINDOW_H
