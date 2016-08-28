#ifndef QDIALOGSETPOINT_H
#define QDIALOGSETPOINT_H

#include <QDialogButtonBox>
#include <QGraphicsItem>
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

#include "qcasusceneitem.h"
#include "qcasutreeitem.h"
#include "dev_msgs.pb.h"

/*!
 * \brief Setpoint configuration dialog window
 *
 * Appearance depends on selected actuator from arenaScene context menu
 */
class QDialogSetpoint : public QDialog
{
    Q_OBJECT
private:
    QRadioButton *radioON;
    QLineEdit* value1;
    QLineEdit* value2;
    QString command_;
    QList<QByteArray> message;

public:
    QDialogSetpoint(QString command, QList<QGraphicsItem *> group);
    QList<QByteArray> getMessage();

private slots:
    void prepareMessage();
    void colorDialog();
};

#endif // QDIALOGSETPOINT_H
