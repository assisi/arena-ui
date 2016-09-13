#ifndef QDIALOGCONNECT_H
#define QDIALOGCONNECT_H

#include <QDialog>

namespace Ui {
class QDialogConnect;
}

class QDialogConnect : public QDialog
{
    Q_OBJECT

public:
    explicit QDialogConnect(QWidget *parent, QStringList addresses, QString name);
    ~QDialogConnect();

    QStringList getAddresses() const;

private:
    Ui::QDialogConnect *ui;
};

#endif // QDIALOGCONNECT_H
