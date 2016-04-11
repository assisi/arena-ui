#ifndef QTREEBUFFER_H
#define QTREEBUFFER_H


#include <QTreeWidgetItem>
#include "qcustomplot.h"
#include "globalHeader.h"

class QTreeBuffer : public QObject, public QTreeWidgetItem
{
    Q_OBJECT
private:

public:
    QCPDataMap* buffer;
    QTime lastDataTime;
    QString legendName;

    explicit QTreeBuffer(const QStringList & strings, QString lName, QWidget* parent = 0);
    ~QTreeBuffer();

    void addToBuffer(QTime time, double value);

signals:
    void updatePlot();

public slots:

};

#endif // QTREEBUFFER_H
