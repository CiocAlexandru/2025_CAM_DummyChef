#include "clientmainwindow.h"
#include "ui_clientmainwindow.h"
#include "recipesearchdialog.h"
#include "shoppinglistdialog.h"
#include "allrecipesdialog.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QResizeEvent>
#include "myordersdialog.h"


ClientMainWindow::ClientMainWindow(const QString& username,const QString& email, QTcpSocket* socket, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ClientMainWindow),
    username(username),
    email(email),
    socket(socket)
{
    ui->setupUi(this);
    backgroundLabel = new QLabel(this);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->lower();

    connect(ui->searchRecipesButton, &QPushButton::clicked, this, &ClientMainWindow::openSearchRecipes);
    connect(ui->shoppingListButton, &QPushButton::clicked, this, &ClientMainWindow::openShoppingList);
    connect(ui->modifyPrefrencesButton, &QPushButton::clicked, this, &ClientMainWindow::openModifyPrefrences);
    connect(ui->viewAllRecipesButton, &QPushButton::clicked, this, &ClientMainWindow::openAllRecipes);
    connect(ui->logoutButton, &QPushButton::clicked,this, &ClientMainWindow::handleLogout);
    connect(ui->viewOrdersButton, &QPushButton::clicked, this, &ClientMainWindow::openOrderHistory);

}

ClientMainWindow::~ClientMainWindow()
{
    delete ui;
}



void ClientMainWindow::openOrderHistory() {
    MyOrdersDialog dialog(email, socket, this);
    dialog.exec();
}


void ClientMainWindow::openSearchRecipes()
{
    RecipeSearchDialog dialog (username,socket,this);
    dialog.exec();
}

void ClientMainWindow::openShoppingList()
{
    ShoppingListDialog* dialog=new ShoppingListDialog(email,socket,this);
    dialog->exec();
}

void ClientMainWindow::openModifyPrefrences()
{
    qDebug() << "[DEBUG] Deschidem ClientPreferencesDialog";
    qDebug() << "[DEBUG] Socket valid?" << (socket != nullptr);
    qDebug() << "[DEBUG] Username:" << username;

    ClientPreferencesDialog dialog(username, socket, this);
    dialog.exec();
    this->close();
}

void ClientMainWindow::openAllRecipes()
{
    AllRecipesDialog dialog(username, socket, this);
    dialog.exec();
}


void ClientMainWindow::updateBackground() {
    QPixmap pixmap(":/images/MainWindowClient.jpg");
    backgroundLabel->setPixmap(pixmap);
    backgroundLabel->setGeometry(0, 0, this->width(), this->height());
}

void ClientMainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    updateBackground();  // Redimensionează fundalul
}

void ClientMainWindow::handleLogout()
{
    if (socket && socket->isOpen()) {
        socket->disconnectFromHost();
    }

    this->close();

    MainWindow *mainWin = new MainWindow();
    mainWin->show();
}
