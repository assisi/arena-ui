#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialogButtonBox>
#include <QDialog>
#include <QString>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>

class connectDialog : public QDialog
{
    Q_OBJECT

public:
    QLineEdit* sub_addr;
    QLineEdit* pub_addr;
    QLineEdit* msg_addr;

    connectDialog(QString sub, QString pub, QString msg);
};

#endif // CONNECTDIALOG_H
