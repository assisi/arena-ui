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
    explicit QDialogSetpoint(QWidget *parent, QString command, QList<QGraphicsItem *> group);
    ~QDialogSetpoint();
    QList<QByteArray> getMessage() const;

private:
    Ui::QDialogSetpoint *ui;

    QString m_command;
    QList<QByteArray> m_message;
    QList<QGraphicsItem *> m_group;

    void prepareMessage();

private slots:
    void sendSetPoint();
    void colorDialog();
};

#endif // QDIALOGSETPOINT_H
