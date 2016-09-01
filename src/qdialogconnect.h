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
    explicit QDialogConnect(QWidget *parent = 0, QStringList addresses = QStringList());
    ~QDialogConnect();

    QStringList getAddresses();

private:
    Ui::QDialogConnect *ui;
};

#endif // QDIALOGCONNECT_H
