#include "qtrendplot.h"

QTrendPlot::QTrendPlot(QTreeWidget* tree, QWidget *parent) :
    QCustomPlot(parent),
    casuTree(tree),
    autoPosition(true),
    showLegend(true),
    docked(true)
{
   QSizePolicy customPolicy(sizePolicy());
   customPolicy.setHeightForWidth(true);
   this->setSizePolicy(customPolicy);

   this->xAxis->setTickLabelType(QCPAxis::ltDateTime);
   this->xAxis->setDateTimeFormat(QString("hh:mm:ss"));

   this->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectLegend | QCP::iSelectPlottables | QCP::iMultiSelect);

   this->setMultiSelectModifier(Qt::ControlModifier);

   this->setAutoAddPlottableToLegend(true);
   this->legend->setVisible(showLegend);
   this->legend->setSelectableParts(QCPLegend::spItems);

   this->setContextMenuPolicy(Qt::CustomContextMenu);

   QCustomPlot::connect(this,&QTrendPlot::mouseDoubleClick,this,&QTrendPlot::enableAutoPosition);
   QCustomPlot::connect(this,&QTrendPlot::mouseMove,this,&QTrendPlot::disableAutoPosition);
   QCustomPlot::connect(this,&QTrendPlot::mouseWheel,this,&QTrendPlot::setZoomFlags);
   QCustomPlot::connect(this,&QTrendPlot::selectionChangedByUser,this,&QTrendPlot::selectionChanged);
   QCustomPlot::connect(this,&QTrendPlot::customContextMenuRequested,this,&QTrendPlot::showContextMenu);
}

void QTrendPlot::addGraph(QTreeBuffer* treeItem){
    this->QCustomPlot::addGraph();
    this->graph()->setData(treeItem->buffer);
    this->graph()->setName(treeItem->legendName);
    this->graph()->setPen(QPen(Qt::black));

    for(int k=7; k < 20; k++){
        bool color_used = false;
        for(int i = 0; i < this->graphCount() ; i++)
            if(this->graph(i)->pen().color() == (Qt::GlobalColor) k) color_used = true;
        if(!color_used){
            this->graph()->setPen(QPen((Qt::GlobalColor) k));
            break;
        }
    }

    QCustomPlot::connect(treeItem,&QTreeBuffer::updatePlot,this,&QTrendPlot::updatePlot);
    connectionMap[this->graph()] = treeItem;
}

void QTrendPlot::addGraphList(QList<QTreeWidgetItem*> itemList)
{    
    bool new_trend = true;
    if(this->graphCount()) new_trend = false;

    for(int k = 0; k < itemList.count(); k++) this->addGraph((QTreeBuffer*)itemList[k]);

    if(new_trend){
        this->rescaleAxes();
        if(this->yAxis->range().size() < 5)this->yAxis->setRange(this->yAxis->range().center(), 5, Qt::AlignCenter);
        if(this->graph()->data()->isEmpty())this->xAxis->setRange(QTime(0,0,0).msecsTo(QTime::currentTime()) /1000, 60, Qt::AlignRight);
        else this->xAxis->setRange(this->graph()->data()->lastKey(), 60, Qt::AlignRight);
    }
}

void QTrendPlot::removeGraph(QCPGraph *graph){
    this->QCustomPlot::removeGraph(graph);
    disconnect(connectionMap[graph],0,this,0);
    connectionMap.remove(graph);
}

void QTrendPlot::removeSelectedGraphs(){
    while(this->selectedGraphs().count())this->removeGraph(this->selectedGraphs().first());
}

void QTrendPlot::addSelectedGraphs(){
    this->addGraphList(casuTree->selectedItems());
}

void QTrendPlot::saveToPDF()
{
    QString path = QFileDialog::getSaveFileName(this,tr("Export trend graph as PDF"),QString(), tr("*.pdf"));
    if(!path.endsWith(".pdf",Qt::CaseInsensitive))path+=".pdf";
    if(path.size())this->savePdf(path);
}

void QTrendPlot::updatePlot(double time, double value){
    double labelAngle = 45 - 30*(this->size().width() - 340)/ this->size().width(); //calculate angle so tick labels are readable
    this->xAxis->setTickLabelRotation(labelAngle > 30 ? labelAngle : 0); //angle is not needed when it is less than 30°

    this->replot();

    if(autoPosition){
        QCPRange yRange = this->yAxis->range();
        QCPRange xRange = this->xAxis->range();

        if(value < yRange.lower || value > yRange.upper)
            this->yAxis->setRange(yRange.center(), abs(yRange.center()-value)*2+4, Qt::AlignCenter);

        this->xAxis->setRange(time + 1, xRange.size(), Qt::AlignRight);
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

void QTrendPlot::selectionChanged(){
    for (int k=0; k<this->graphCount(); k++){
        QCPGraph *graph = this->graph(k);
        QCPPlottableLegendItem *item = this->legend->itemWithPlottable(graph);
        if (item->selected() || graph->selected()){
          item->setSelected(true);
          graph->setSelected(true);
        }
      }
}

void QTrendPlot::showContextMenu(QPoint pos){
    QMenu *menu = new QMenu(this);
    QAction* temp;

    menu->setAttribute(Qt::WA_DeleteOnClose);

    menu->addAction((docked? "Undock from main window" : "Dock to main window"),this,SLOT(dock_undock()));

    menu->addAction((showLegend? "Hide legend" : "Show legend"),this,SLOT(toggleLegend()));

    temp = menu->addAction("Remove selected graphs",this,SLOT(removeSelectedGraphs()));
    if(!this->selectedGraphs().count()) temp->setEnabled(false);

    temp=menu->addAction("Add graphs (selected in tree)",this,SLOT(addSelectedGraphs()));
    if(!casuTree->selectedItems().count()) temp->setEnabled(false);

    menu->addAction("Save to pdf",this,SLOT(saveToPDF()));

    menu->addAction("Close trend",this,SLOT(close()));



    menu->popup(this->mapToGlobal(pos));
}

void QTrendPlot::toggleLegend(){
    showLegend = !showLegend;
    this->legend->setVisible(showLegend);
}

void QTrendPlot::dock_undock(){
    if(docked)this->setParent(this->parentWidget(), Qt::Window);
    if(!docked)this->setParent(this->parentWidget(), Qt::Widget);
    this->show();
    docked = !docked;
}
