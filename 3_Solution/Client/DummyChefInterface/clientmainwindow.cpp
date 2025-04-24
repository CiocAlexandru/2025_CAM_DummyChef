#include "clientmainwindow.h"
#include "ui_clientmainwindow.h"
#include "recipesearchdialog.h"
#include "shoppinglistdialog.h"
#include <QMessageBox>

ClientMainWindow::ClientMainWindow(const QString& username, QTcpSocket* socket, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ClientMainWindow),
    username(username),
    socket(socket)
{
    ui->setupUi(this);
    backgroundLabel = new QLabel(this);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->lower();

    connect(ui->searchRecipesButton, &QPushButton::clicked, this, &ClientMainWindow::openSearchRecipes);
    connect(ui->shoppingListButton, &QPushButton::clicked, this, &ClientMainWindow::openShoppingList);
    connect(ui->modifyProfileButton, &QPushButton::clicked, this, &ClientMainWindow::openModifyProfile);


}

ClientMainWindow::~ClientMainWindow()
{
    delete ui;
}

void ClientMainWindow::openSearchRecipes()
{
    RecipeSearchDialog dialog (username,socket,this);
    dialog.exec();
}

void ClientMainWindow::openShoppingList()
{
    ShoppingListDialog* dialog=new ShoppingListDialog(username,socket,this);
    dialog->exec();
}

void ClientMainWindow::openModifyProfile()
{

    ClientPreferencesDialog dialog(username, socket, this);
    dialog.exec(); // deschide modal
}

void ClientMainWindow::updateBackground() {
    QPixmap pixmap(":/images/MainWindowClient.jpg");  // Încarcă imaginea din resurse
    backgroundLabel->setPixmap(pixmap);
    backgroundLabel->setGeometry(0, 0, this->width(), this->height());  // Acoperă întreaga fereastră
}

void ClientMainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    setWindowState(windowState() | Qt::WindowFullScreen);
    updateBackground();
}

