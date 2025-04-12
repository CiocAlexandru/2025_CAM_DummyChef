#include "clientpreferencesdialog.h"
#include "ui_clientpreferencesdialog.h"
#include <QMessageBox>

ClientPreferencesDialog::ClientPreferencesDialog(const QString& username, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClientPreferencesDialog),
    socket(new QTcpSocket(this)),
    backgroundLabel(new QLabel(this)),  // Adăugat inițializare
    username(username)  // Corect - folosește lista de inițializare
{
    ui->setupUi(this);

    // Configurare backgroundLabel
    backgroundLabel->setScaledContents(true);
    backgroundLabel->lower();

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
    QString message = QString("PREFERINTE_CLIENT %1 %2 %3 %4 %5 %6")
    .arg(username) // Atenție: trebuie să-l primești în constructor
        .arg(foodPreferences)
        .arg(allergies)
        .arg(deliveryTime)
        .arg(pricePreference)
        .arg(notes);

    socket->write(message.toUtf8());
}

void ClientPreferencesDialog::handleSavePreferences()
{
    QString foodPreferences = ui->foodPreferencesEdit->toPlainText().trimmed();
    QString allergies = ui->allergiesEdit->text().trimmed();
    QString deliveryTime = ui->deliveryTimeCombo->currentText();
    QString pricePreference = ui->pricePreferenceCombo->currentText();
    QString notes = ui->notesEdit->toPlainText().trimmed();

    // Aici ai putea trimite datele la server sau salva local
    socket->connectToHost("127.0.0.1", 12345);
    QMessageBox::information(this, "Preferințe Salvate", "Preferințele au fost salvate cu succes!");
    accept();
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
}
