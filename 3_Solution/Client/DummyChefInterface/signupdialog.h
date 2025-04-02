#ifndef SIGNUPDIALOG_H
#define SIGNUPDIALOG_H

#include <QDialog>

#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui {class SignUpDialog;}
QT_END_NAMESPACE


class SignUpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SignUpDialog(QWidget *parent = nullptr);
    ~SignUpDialog();

protected:
    void resizeEvent(QResizeEvent *event) override;  // Suprascriem resizeEvent pentru fundal



private:
    Ui::SignUpDialog *ui;
    QLabel *backgroundLabel;  // Adăugăm QLabel pentru fundal

    void updateBackground();
};

#endif // SIGNUPDIALOG_H
