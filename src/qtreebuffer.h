#ifndef QTREEBUFFER_H
#define QTREEBUFFER_H


#include <QTreeWidgetItem>
#include "QCustomPlot/qcustomplot.h"
#include "globalHeader.h"

/*!
 * \brief Custom subclassed QTreeWidgetItem and Qobject for use in casuTree
 *
 * Keeps buffer of data values received from CASU sensors and has ability to connect to slots as QObject
 */
class QTreeBuffer : public QObject, public QTreeWidgetItem
{
    Q_OBJECT
private:

public:
    /*!
     * \brief Buffer of data values which is ploted with QTrendPlot
     *
     * For this kind of use, QCustomPlot behaviour was changed so it doesn't claim parentage over QCPDataMap pointer so stored buffer data remain intact after closing QTrendPlot
     */
    QCPDataMap* buffer;
    /*!
     * \brief Variable to keep track of acquisition time of latest data value
     */
    QTime lastDataTime;
    /*!
     * \brief Name of data stored in buffer
     */
    QString legendName;

    explicit QTreeBuffer(const QStringList & strings, QString lName, QWidget* parent = 0);
    ~QTreeBuffer();

    /*!
     * \brief Function to add new value in buffer with coresponding acquisition time
     * \param time
     * \param value
     */
    void addToBuffer(QTime time, double value);

signals:
    /*!
     * \brief Signal which is emitted when new value is received and QTrendPlot needs to be updated.
     */
    void updatePlot();

public slots:

};

#endif // QTREEBUFFER_H
