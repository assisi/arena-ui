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

namespace Ui{
    class QDialogSetpoint;
}

/*!
 * \brief Setpoint configuration dialog window
 *
 * Appearance depends on selected actuator from arenaScene context menu
 */
class QDialogSetpoint : public QDialog
{
    Q_OBJECT
public:
    QDialogSetpoint(QWidget *parent, QString command, QList<QGraphicsItem *> group);
    QList<QByteArray> getMessage();

private:
    Ui::QDialogSetpoint *ui;

    QString _command;
    QList<QByteArray> _message;

private slots:
    void prepareMessage();
    void colorDialog();
};

#endif // QDIALOGSETPOINT_H
