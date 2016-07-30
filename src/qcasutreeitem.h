#ifndef CASUTREEITEM_H
#define CASUTREEITEM_H

#include <fstream>
#include <QTreeWidgetItem>

#include <boost/lexical_cast.hpp>
#include <boost/functional/hash.hpp>

#include <nzmqt/nzmqt.hpp>
#include "dev_msgs.pb.h"
#include "globalHeader.h"
#include "qtreebuffer.h"

using namespace nzmqt;
using namespace AssisiMsg;
using namespace boost;
using namespace std;

class QCasuTreeItem : public QObject, public QTreeWidgetItem
{
    Q_OBJECT

private:
    nzmqt::ZMQContext* context_;
    nzmqt::ZMQSocket* pub_sock_;
    nzmqt::ZMQSocket* sub_sock_;

    QTimer* connection_timer;

    // Connect the publisher and subscriber
    void connect_();

    QString log_name;
    bool log_open;
    ofstream log_file;
    void openLogFile();
    void closeLogFile();

public:
    QString casuName;

    bool connected;
    bool led_on;
    bool child_selected;
    bool airflowON;
    bool vibrON;

    QColor led_color;

    QString sub_addr;
    QString pub_addr;
    QString msg_addr;

    //pointers to tree objects needs to be saved because sorting changes children order

    QTreeWidgetItem *widget_IR;
    QTreeWidgetItem *widget_LED;
    QTreeWidgetItem *widget_temp;
    QTreeWidgetItem *widget_vibr;
    QTreeWidgetItem *widget_setpoints;

    QList<QTreeWidgetItem*> widget_IR_children;
    QList<QTreeWidgetItem*> widget_temp_children;
    QList<QTreeWidgetItem*> widget_vibr_children;
    QList<QTreeWidgetItem*> widget_setpoints_children;
    QList<QTreeWidgetItem*> widget_setpoints_vibr_children;

    QCasuTreeItem(QObject *parent, QString name);

    void setAddr(QString sub, QString pub, QString msg);
    bool sendSetpoint(QList<QByteArray> message);

    void resetSelection();

signals:

private slots:
    void updateSelection();

    void messageReceived(const QList<QByteArray>& message);

    void connectionTimeout();
};

#endif // CASUTREEITEM_H
