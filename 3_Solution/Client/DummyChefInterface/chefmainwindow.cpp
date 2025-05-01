// chefmainwindow.cpp
#include "chefmainwindow.h"
#include "ui_chefmainwindow.h"
#include "addrecipedialog.h"
#include "myrecipesdialog.h"
#include "addingredientdialog.h"

#include <QDebug>
#include <QMessageBox>

ChefMainWindow::ChefMainWindow(const QString& email, QTcpSocket* socket, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ChefMainWindow),
    socket(socket),
    email(email)
{
    ui->setupUi(this);
    backgroundLabel = new QLabel(this);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->lower();

    connect(ui->addRecipeButton, &QPushButton::clicked, this, &ChefMainWindow::openAddRecipe);
    connect(ui->myRecipesButton, &QPushButton::clicked, this, &ChefMainWindow::openMyRecipes);
    connect(ui->addIngredientButton, &QPushButton::clicked, this, &ChefMainWindow::openAddIngredient);
}

ChefMainWindow::~ChefMainWindow()
{
    delete ui;
}

void ChefMainWindow::openAddRecipe()
{
    AddRecipeDialog* dialog=new AddRecipeDialog(email, socket, this);
    dialog->exec();
    //dialog->
}

void ChefMainWindow::openMyRecipes()
{
    MyRecipesDialog dialog(email, socket, this);
    dialog.exec();
}

void ChefMainWindow::openAddIngredient()
{
    AddIngredientDialog dialog(email, socket, this);
    dialog.exec();
}

void ChefMainWindow::updateBackground() {
    QPixmap pixmap(":/images/ChefMainWindow.jpg");
    backgroundLabel->setPixmap(pixmap);
    backgroundLabel->setGeometry(0, 0, this->width(), this->height());
}

void ChefMainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    updateBackground();
}
