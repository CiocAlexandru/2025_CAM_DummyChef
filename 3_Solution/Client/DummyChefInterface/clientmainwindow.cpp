#include "clientmainwindow.h"
#include "ui_clientmainwindow.h"
#include "recipesearchdialog.h"
#include "shoppinglistdialog.h"
#include "allrecipesdialog.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QResizeEvent>


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

void ClientMainWindow::openModifyPrefrences()
{
    qDebug() << "[DEBUG] Deschidem ClientPreferencesDialog";
    qDebug() << "[DEBUG] Socket valid?" << (socket != nullptr);
    qDebug() << "[DEBUG] Username:" << username;

    ClientPreferencesDialog dialog(username, socket, this);
    dialog.exec(); // deschide modal
}

void ClientMainWindow::openAllRecipes()
{
    AllRecipesDialog dialog(username, socket, this);
    dialog.exec();
}


void ClientMainWindow::updateBackground() {
    QPixmap pixmap(":/images/MainWindowClient.jpg");  // Încarcă imaginea din resurse
    backgroundLabel->setPixmap(pixmap);
    backgroundLabel->setGeometry(0, 0, this->width(), this->height());  // Acoperă întreaga fereastră
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

    this->close();  // Închide fereastra curentă

    MainWindow *mainWin = new MainWindow();  // Creează o nouă instanță
    mainWin->show();  // Afișează fereastra principală
}
