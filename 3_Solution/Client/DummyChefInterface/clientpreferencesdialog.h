#ifndef CLIENTPREFERENCEDIALOG_H
#define CLIENTPREFERENCEDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QTcpSocket>

namespace Ui {
class ClientPreferencesDialog;
}

class ClientPreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClientPreferencesDialog(const QString& username,QWidget *parent = nullptr);
    ~ClientPreferencesDialog();

private slots:
    void handleSavePreferences();
    void onReadyRead();
    void onConnected();
    void onError(QAbstractSocket::SocketError socketError);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::ClientPreferencesDialog *ui;
    QLabel *backgroundLabel;
    void updateBackground();
    QString username;  // Adăugat pentru a stoca username-ul primit în constructor
    QString foodPreferences;
    QString allergies;
    QString deliveryTime;
    QString pricePreference;
    QString notes;
    QTcpSocket*socket;
};

#endif // CLIENTPREFERENCEDIALOG_H
