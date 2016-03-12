#ifndef QTREEBUFFER_H
#define QTREEBUFFER_H


#include <QTreeWidgetItem>
#include "qcustomplot.h"
#include "global.h"

class QTreeBuffer : public QObject, public QTreeWidgetItem
{
    Q_OBJECT
private:

public:
    QCPDataMap* buffer;
    QTime lastDataTime;

    explicit QTreeBuffer(const QStringList & strings, QWidget* parent = 0);
    ~QTreeBuffer();

    void addToBuffer(QTime time, double value);

signals:
    void updatePlot(double time, double value);

public slots:

};

#endif // QTREEBUFFER_H
