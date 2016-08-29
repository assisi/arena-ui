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
private:
    QLineEdit* _subAddr;
    QLineEdit* _pubAddr;
    QLineEdit* _msgAddr;
public:
    QDialogConnect(QStringList addresses);
    QStringList getAddresses();
};

#endif // QDIALOGCONNECT_H
