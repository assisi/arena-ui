#ifndef QCONNECTDIALOG_H
#define QCONNECTDIALOG_H

#include <QDialogButtonBox>
#include <QDialog>
#include <QString>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>

class QConnectDialog : public QDialog
{
    Q_OBJECT

public:
    QLineEdit* sub_addr;
    QLineEdit* pub_addr;
    QLineEdit* msg_addr;

    QConnectDialog(QString sub, QString pub, QString msg);
};

#endif // QCONNECTDIALOG_H
