#ifndef SIGNUPDIALOG_H
#define SIGNUPDIALOG_H

#include <QDialog>
#include <QTcpSocket>
#include <QLabel>
#include "clientsignupdialog.h"  // Fereastra pentru client
#include "chefsignupdialog.h"
QT_BEGIN_NAMESPACE
namespace Ui {class SignUpDialog;}
QT_END_NAMESPACE


class SignUpDialog : public QDialog
{
    Q_OBJECT

signals:
    void userTypeSelected(const QString &type);

public:
    explicit SignUpDialog(QWidget *parent = nullptr);
    ~SignUpDialog();

protected:
    void resizeEvent(QResizeEvent *event) override;  // Suprascriem resizeEvent pentru fundal

private slots:
    void on_clientButton_clicked();
    void on_bucatarButton_clicked();

private:
    Ui::SignUpDialog *ui;
    QLabel *backgroundLabel;  // Adăugăm QLabel pentru fundal
    QTcpSocket *socket;
    void updateBackground();
};

#endif // SIGNUPDIALOG_H
