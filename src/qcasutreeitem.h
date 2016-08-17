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

/*!
 * \brief QTreeWidget element with NZMQt protocol for individual CASU
 */
class QCasuTreeItem : public QObject, public QTreeWidgetItem
{
    Q_OBJECT

private:
    nzmqt::ZMQContext* context_;
    nzmqt::ZMQSocket* pub_sock_;
    nzmqt::ZMQSocket* sub_sock_;

    /*!
     * \brief If timer runs out without receiving any message, connection is innactive
     */
    QTimer* connectionTimer;

    // Connect the publisher and subscriber
    /*!
     * \brief Connect NZMQt publisher and subscriber
     */
    void connect_();

    /*!
     * \brief logFile name of this CASU
     */
    QString logName;
    /*!
     * \brief Status variable if log file is open
     */
    bool logOpen;
    /*!
     * \brief Ofstream log file
     */
    ofstream logFile;
    /*!
     * \brief Opens logFile stream if neccesary
     */
    void openLogFile();
    /*!
     * \brief Closes logFile stream when not needed
     */
    void closeLogFile();

public:
    QString casuName;

    bool connected;
    bool ledON;
    bool child_selected;
    bool airflowON;
    bool vibrON;

    QColor ledColor;

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

    /*!
     * \brief Change connection parameters
     * \param sub - subscriber IP address
     * \param pub - publisher IP address
     * \param msg - messaging IP address
     */
    void setAddr(QString sub, QString pub, QString msg);
    /*!
     * \brief Sends the setpoint message to this items CASU
     * \param message - protobuf message
     */
    bool sendSetpoint(QList<QByteArray> message);

    /*!
     * \brief Deselects all tree items in treeView
     */
    void resetSelection();

signals:

private slots:
    /*!
     * \brief Updates selected tree items in treeView
     */
    void updateSelection();

    /*!
     * \brief Message receiver and parser
     * \param message - protobuf message
     */
    void messageReceived(const QList<QByteArray>& message);

    /*!
     * \brief Resets connection status if connectionTimer runs out
     */
    void connectionTimeout();
};

#endif // CASUTREEITEM_H
