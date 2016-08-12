#ifndef QDIALOGSETTINGS_H
#define QDIALOGSETTINGS_H

#include <QDialogButtonBox>
#include <QDialog>
#include <QIcon>
#include <QPushButton>
#include <QFileDialog>
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QMap>

#include "globalHeader.h"

/*!
 * \brief Subclassed instance of QLabel
 *
 * Configured aligmnent options beforehand to use in QDialogSettings
 */
class QtempLabel : public QLabel
{
    Q_OBJECT
public:
    QtempLabel(QString label);
};

/*!
 * \brief Program settings dialog window
 */
class QDialogSettings : public QDialog
{
    Q_OBJECT

public:
       QDialogSettings();

private:
       QLineEdit* logFolder;
       QLineEdit* camFolder;
       QLineEdit* arenaFolder;
       QLineEdit* simFile;
       QDateTimeEdit* trendTimeSpan;
       QLineEdit* trendSampleTime_ms;
       QCheckBox* forceLog;
       QCheckBox* forceScene;


       QMap<QPushButton*,QLineEdit*> link;

private slots:
       void findFolder();
       void saveConfig();
};

#endif // QDIALOGSETTINGS_H
