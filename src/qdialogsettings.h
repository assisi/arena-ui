#ifndef QDIALOGSETTINGS_H
#define QDIALOGSETTINGS_H

#include <QDialog>
#include <QMap>
#include <QLineEdit>
#include <QFileDialog>

#include "globalHeader.h"

namespace Ui {
class QDialogSettings;
}

class QDialogSettings : public QDialog
{
    Q_OBJECT

public:
       explicit QDialogSettings(QWidget *parent = 0);
       ~QDialogSettings();

private:
       Ui::QDialogSettings *ui;

       QMap<QPushButton*,QLineEdit*> link;

private slots:
       void findFolder();
       void saveConfig();
};

#endif // QDIALOGSETTINGS_H
