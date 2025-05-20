#ifndef CLIENTPREFERENCESDIALOG_H
#define CLIENTPREFERENCESDIALOG_H

#include <QDialog>
#include <QTcpSocket>
#include <QLabel>

namespace Ui {
class ClientPreferencesDialog;
}

class ClientPreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClientPreferencesDialog(const QString& username, QTcpSocket* existingSocket, QWidget *parent = nullptr);
    ~ClientPreferencesDialog();

private slots:
    void sendPreferences();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError socketError);
    void handleSavePreferences();

private:
    void updateBackground();
    void resizeEvent(QResizeEvent *event) override;

    Ui::ClientPreferencesDialog *ui;
    QTcpSocket *socket;
    bool raspunsPrimit;
    QLabel *backgroundLabel;
    QString username;
    QString foodPreferences;
    QString allergies;
    QString deliveryTime;
    QString pricePreference;
    QString notes;
};

#endif // CLIENTPREFERENCESDIALOG_H
