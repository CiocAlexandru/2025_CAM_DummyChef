#include "clientpreferencesdialog.h"
#include "ui_clientpreferencesdialog.h"
#include <QMessageBox>

ClientPreferencesDialog::ClientPreferencesDialog(const QString& username, QTcpSocket* existingSocket, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClientPreferencesDialog),
    socket(existingSocket),  // Use the passed socket instead of creating a new one
    backgroundLabel(new QLabel(this)),
    username(username)
{
    ui->setupUi(this);

    // Configurare backgroundLabel
    backgroundLabel->setScaledContents(true);
    backgroundLabel->lower();

    // Configurare câmpuri și butoane
    ui->foodPreferencesEdit->setPlaceholderText("Introduceți preferințele alimentare");
    ui->allergiesEdit->setPlaceholderText("Introduceți alergiile");
    ui->notesEdit->setPlaceholderText("Introduceți note suplimentare");

    // Configurare butoane
    ui->saveButton->setText("Salvează Preferințe");

    // Populare combobox-uri
    ui->deliveryTimeCombo->addItems({"Orice_ora", "Dimineata(8-12)", "Pranz(12-15)", "Seara(15-20)", "Noapte(20-24)"});
    ui->pricePreferenceCombo->addItems({"Orice_pret", "Economic", "Moderat", "Premium"});

    // Conectare semnale
    connect(socket, &QTcpSocket::connected, this, &ClientPreferencesDialog::onConnected);
    connect(socket, &QTcpSocket::readyRead, this, &ClientPreferencesDialog::onReadyRead);
    connect(socket, &QTcpSocket::errorOccurred, this, &ClientPreferencesDialog::onError);

    setWindowTitle("Preferințe Client");
    connect(ui->saveButton, &QPushButton::clicked, this, &ClientPreferencesDialog::handleSavePreferences);

    updateBackground();
}

void ClientPreferencesDialog::onReadyRead()
{
    QString response = QString::fromUtf8(socket->readAll()).trimmed();

    if (response == "PREFERINTE_CLIENT_SUCCESS") {
        QMessageBox::information(this, "Succes", "Preferințele au fost salvate cu succes!");
        accept();
    } else {
        QMessageBox::warning(this, "Eroare", "Salvarea preferințelor a eșuat!");
    }
}

void ClientPreferencesDialog::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    QMessageBox::critical(this, "Eroare conexiune", socket->errorString());
}

void ClientPreferencesDialog::onConnected()
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

    socket->write(message.toUtf8());
}

void ClientPreferencesDialog::handleSavePreferences()
{
    // Validare câmpuri
    if (ui->foodPreferencesEdit->toPlainText().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Eroare", "Introduceți preferințele alimentare!");
        return;
    }

    // If the socket is already connected, send the message directly
    if (socket->state() == QAbstractSocket::ConnectedState) {
        onConnected();
    } else {
        // If not connected, attempt to connect
        socket->connectToHost("127.0.0.1", 12345);
    }
}

void ClientPreferencesDialog::updateBackground() {
    QPixmap pixmap(":/images/ClientPreference.jpg");  // Încarcă imaginea din resurse
    backgroundLabel->setPixmap(pixmap);
    backgroundLabel->setGeometry(0, 0, this->width(), this->height());  // Acoperă întreaga fereastră
}

void ClientPreferencesDialog::resizeEvent(QResizeEvent *event) {
    QDialog::resizeEvent(event);
    setWindowState(windowState() | Qt::WindowFullScreen);
    updateBackground();  // Actualizează dimensiunea fundalului la redimensionare
}

ClientPreferencesDialog::~ClientPreferencesDialog()
{
    delete ui;
    // Do not delete the socket here since it's owned by ClientSignUpDialog
}
