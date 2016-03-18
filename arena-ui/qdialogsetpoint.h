#ifndef QDIALOGSETPOINT_H
#define QDIALOGSETPOINT_H

#include <QDialogButtonBox>
#include <QDialog>
#include <QString>
#include <QRadioButton>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QRegularExpressionValidator>
#include <QPushButton>
#include <QColorDialog>

#include <iostream>

#include "dev_msgs.pb.h"

class QDialogSetpoint : public QDialog
{
    Q_OBJECT
private:
    QRadioButton *radioON;
    QLineEdit* value1;
    QLineEdit* value2;
    QString command_;

public:
    QList<QByteArray> message;

    QDialogSetpoint(QString command);

private slots:
    void prepareMessage();
    void colorDialog();
};

#endif // QDIALOGSETPOINT_H
