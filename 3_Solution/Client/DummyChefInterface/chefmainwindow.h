// chefmainwindow.h
#ifndef CHEFMAINWINDOW_H
#define CHEFMAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QLabel>

namespace Ui {
class ChefMainWindow;
}

class ChefMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ChefMainWindow(const QString& email, QTcpSocket* socket, QWidget *parent = nullptr);
    ~ChefMainWindow();

private slots:
    void openAddRecipe();
    void openMyRecipes();
    void openAddIngredient();
    void handleLogout();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::ChefMainWindow *ui;
    QLabel *backgroundLabel;
    QTcpSocket* socket;
    QString email;

    void updateBackground();
};

#endif // CHEFMAINWINDOW_H
