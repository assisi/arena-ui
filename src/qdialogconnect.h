#ifndef QDIALOGCONNECT_H
#define QDIALOGCONNECT_H

#include <QDialogButtonBox>
#include <QDialog>
#include <QString>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>

/*!
 * \brief Connection settings dialog window for individual CASU
 */
class QDialogConnect : public QDialog
{
    Q_OBJECT

public:
    QLineEdit* sub_addr;
    QLineEdit* pub_addr;
    QLineEdit* msg_addr;

    QDialogConnect(QString sub, QString pub, QString msg);
};

#endif // QDIALOGCONNECT_H
