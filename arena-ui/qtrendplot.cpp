#include "qtrendplot.h"

QTrendPlot::QTrendPlot(QWidget *parent) :
    QCustomPlot(parent),
    autoPosition(true)
{
   QSizePolicy customPolicy(sizePolicy());
   customPolicy.setHeightForWidth(true);
   this->setSizePolicy(customPolicy);

   this->xAxis->setTickLabelType(QCPAxis::ltDateTime);
   this->xAxis->setDateTimeFormat(QString("hh:mm:ss"));

   this->setInteraction(QCP::iRangeDrag, true);
   this->setInteraction(QCP::iRangeZoom, true);

   QCustomPlot::connect(this,&QTrendPlot::mouseDoubleClick,this,&QTrendPlot::enableAutoPosition);
   QCustomPlot::connect(this,&QTrendPlot::mouseMove,this,&QTrendPlot::disableAutoPosition);
   QCustomPlot::connect(this,&QTrendPlot::mouseWheel,this,&QTrendPlot::setZoomFlags);
}

void QTrendPlot::addCurve(QTreeBuffer* treeItem){
    this->addGraph();
    this->graph()->setData(treeItem->buffer);

    QCustomPlot::connect(treeItem,&QTreeBuffer::updatePlot,this,&QTrendPlot::updatePlot);
}

void QTrendPlot::removeCurve(){}

void QTrendPlot::updatePlot(double time, double value){
    double labelAngle = 45 - 30*(this->size().width() - 340)/ this->size().width(); //calculate angle so tick labels are readable
    this->xAxis->setTickLabelRotation(labelAngle > 30 ? labelAngle : 0); //angle is not needed when it is less than 30°

    this->replot();

    if(autoPosition){
        QCPRange yRange = this->yAxis->range();
        QCPRange xRange = this->xAxis->range();

        if(value < yRange.lower || value > yRange.upper)
            this->yAxis->setRange(yRange.center(), abs(yRange.center()-value)+4, Qt::AlignCenter);

        this->xAxis->setRange(time, xRange.size(), Qt::AlignRight);
    }
}

void QTrendPlot::enableAutoPosition(){
    autoPosition = true;
}

void QTrendPlot::disableAutoPosition(QMouseEvent *event){
    if(event->buttons() == Qt::LeftButton)autoPosition = false;
}

void QTrendPlot::setZoomFlags(QWheelEvent *event){
    if(event->modifiers() & Qt::ShiftModifier){
        this->axisRect()->setRangeZoom(Qt::Horizontal);
    }
    else
    if(event->modifiers() & Qt::ControlModifier){
        this->axisRect()->setRangeZoom(Qt::Vertical);
    }
    else{
        this->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    }
}
