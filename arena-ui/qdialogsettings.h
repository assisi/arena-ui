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

class QtempLabel : public QLabel
{
    Q_OBJECT
public:
    QtempLabel(QString label);
};

class QDialogSettings : public QDialog
{
    Q_OBJECT

public:
       QDialogSettings();

private:
       QLineEdit* logFolder;
       QLineEdit* camFolder;
       QLineEdit* arenaFolder;
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
