#include "clientpreferencesdialog.h"
#include "ui_clientpreferencesdialog.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QDebug>

ClientPreferencesDialog::ClientPreferencesDialog(const QString& username, QTcpSocket* existingSocket, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClientPreferencesDialog),
    socket(existingSocket),
    backgroundLabel(new QLabel(this)),
    username(username),
    raspunsPrimit(false)
{
    ui->setupUi(this);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->lower();

    ui->foodPreferencesEdit->setPlaceholderText("Introduceți preferințele alimentare");
    ui->allergiesEdit->setPlaceholderText("Introduceți alergiile");
    ui->notesEdit->setPlaceholderText("Introduceți note suplimentare");

    ui->saveButton->setText("Salvează Preferințe");

    ui->deliveryTimeCombo->addItems({"Orice_ora", "Dimineata(8-12)", "Pranz(12-15)", "Seara(15-20)", "Noapte(20-24)"});
    ui->pricePreferenceCombo->addItems({"Orice_pret", "Economic", "Moderat", "Premium"});

    connect(socket, &QTcpSocket::readyRead, this, &ClientPreferencesDialog::onReadyRead);
    connect(socket, &QTcpSocket::errorOccurred, this, &ClientPreferencesDialog::onError);

    setWindowTitle("Preferințe Client");

    connect(ui->saveButton, &QPushButton::clicked, this, &ClientPreferencesDialog::handleSavePreferences);

    updateBackground();
}

void ClientPreferencesDialog::handleSavePreferences()
{
    if (ui->foodPreferencesEdit->toPlainText().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Eroare", "Introduceți preferințele alimentare!");
        return;
    }

    sendPreferences();
}

void ClientPreferencesDialog::sendPreferences()
{
    foodPreferences = ui->foodPreferencesEdit->toPlainText().trimmed();
    allergies = ui->allergiesEdit->text().trimmed();
    deliveryTime = ui->deliveryTimeCombo->currentText();
    pricePreference = ui->pricePreferenceCombo->currentText();
    notes = ui->notesEdit->toPlainText().trimmed();

    QString message = QString("PREFERINTE_CLIENT %1 %2 %3 %4 %5 %6")
                          .arg(username)
                          .arg(foodPreferences)
                          .arg(allergies)
                          .arg(deliveryTime)
                          .arg(pricePreference)
                          .arg(notes);

    qDebug() << "Trimitem la server:" << message;

    socket->write(message.toUtf8());
    socket->flush();

    socket->waitForBytesWritten(3000);


    if (!socket->waitForReadyRead(3000)) {
        QMessageBox::critical(this, "Eroare", "Serverul nu a răspuns în timp util.");
        return;
    }

    if(raspunsPrimit==false)
    {
        onReadyRead();
    }
}

void ClientPreferencesDialog::onReadyRead()
{
    QString response = QString::fromUtf8(socket->readAll()).trimmed();
    qDebug() << "Răspuns de la server:" << response;

    if (response == "PreferinteSucces") {
        QMessageBox::information(this, "Succes", "Preferințele au fost salvate cu succes!");
        QMessageBox::information(this, "Atentie!", "Trebuie sa va logati din nou in cont pentru a fi actualizate modificarile.");
        raspunsPrimit = true;

        if (socket && socket->isOpen()) {
            socket->abort();
        }
        this->close();

        MainWindow* mainWin = new MainWindow();
        mainWin->show();
    }
     else {
        QMessageBox::warning(this, "Eroare", "Salvarea preferințelor a eșuat!");
    }
}

void ClientPreferencesDialog::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    QMessageBox::critical(this, "Eroare conexiune", socket->errorString());
}

void ClientPreferencesDialog::updateBackground()
{
    QPixmap pixmap(":/images/ClientPreference.jpg");
    backgroundLabel->setPixmap(pixmap);
    backgroundLabel->setGeometry(0, 0, this->width(), this->height());
}

void ClientPreferencesDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    setWindowState(windowState() | Qt::WindowFullScreen);
    updateBackground();
}

ClientPreferencesDialog::~ClientPreferencesDialog()
{
    delete ui;
}
