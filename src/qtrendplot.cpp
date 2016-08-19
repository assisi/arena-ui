#include "qtrendplot.h"

QTrendPlot::QTrendPlot(QGraphicsScene* scene, QTreeWidget* tree1,QTreeWidget* tree2 , QWidget *parent) :
    QCustomPlot(parent),
    casuTree(tree1),
    groupTree(tree2),
    arenaScene(scene),
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

   connect(this,&QTrendPlot::mouseDoubleClick,this,&QTrendPlot::enableAutoPosition);
   connect(this,&QTrendPlot::mouseMove,this,&QTrendPlot::disableAutoPosition);
   connect(this,&QTrendPlot::mouseWheel,this,&QTrendPlot::setZoomFlags);
   connect(this,&QTrendPlot::selectionChangedByUser,this,&QTrendPlot::selectionChanged);
   connect(this,&QTrendPlot::customContextMenuRequested,this,&QTrendPlot::showContextMenu);
   connect(this,&QTrendPlot::beforeReplot,this,&QTrendPlot::prettyPlot);
}

void QTrendPlot::addGraph(QTreeBuffer* treeItem){
    for(int k=0; k<this->graphCount(); k++){
        if (!QString::compare(this->graph(k)->name(), treeItem->legendName)) return;
    }
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

    QCustomPlot::connect(treeItem,SIGNAL(updatePlot()),this,SLOT(replot()));
    connectionMap[this->graph()] = treeItem;
}

bool sortQTreeWidgetItem(QTreeWidgetItem* item1,QTreeWidgetItem* item2){
    return QString::compare(((QTreeBuffer*)item1)->legendName,((QTreeBuffer*)item2)->legendName) < 0;
}
void QTrendPlot::addGraphList(QList<QTreeWidgetItem*> itemList)
{    
    bool new_trend = true;
    if(this->graphCount()) new_trend = false;

    itemList = itemList.toSet().toList(); //remove duplicates
    qSort(itemList.begin(),itemList.end(),sortQTreeWidgetItem); //sort by legend name

    foreach (QTreeWidgetItem* item, itemList) this->addGraph((QTreeBuffer*)item);

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
    QList<QTreeWidgetItem*> selectedList = casuTree->selectedItems();

    foreach(QTreeWidgetItem* item, groupTree->selectedItems()){
        QTreeWidgetItem* parent = item->parent();
        while(parent->parent()) parent = parent->parent();
        QColor color = parent->textColor(0);
        QString name = item->text(0);
        QString parentName = parent->text(0);

        foreach (QGraphicsItem* casuItem, arenaScene->items()) {
            if(casuItem->childItems().size()) continue;
            if(((QCasuSceneItem*)casuItem)->groupColor != color && !QString::compare(parentName, "CASU group")) continue;
            if(!casuItem->isSelected() && !QString::compare(parentName, "Selected CASUs")) continue;
            selectedList.append(((QCasuSceneItem*)casuItem)->treeItem->widgetMap[name]);
        }
    }
    this->addGraphList(selectedList);

    this->replot();
}

void QTrendPlot::saveToPDF()
{
    QString path = QFileDialog::getSaveFileName(this,tr("Export trend graph as PDF"),QString(), tr("*.pdf"));
    if(!path.endsWith(".pdf",Qt::CaseInsensitive))path+=".pdf";
    if(path.size())this->savePdf(path);
}

void QTrendPlot::prettyPlot()
{
    //it is called before replot() to tidy everything
    double labelAngle = 45 - 30*(this->size().width() - 340)/ this->size().width(); //calculate angle so tick labels are readable
    this->xAxis->setTickLabelRotation(labelAngle > 30 ? labelAngle : 0); //angle is not needed when it is less than 30°

    if(autoPosition){
        QCPRange yRange = this->yAxis->range();
        QCPRange xRange = this->xAxis->range();

        QCPData temp;
        temp.key = 0;

        for(int k = 0; k < this->graphCount();k++){
            QCPDataMap* tempMap = this->graph(k)->data();
            if(tempMap->isEmpty()) continue;
            double tempKey = tempMap->lastKey();
            if(k==0 || temp.key < tempKey) temp = tempMap->find(tempKey).value();
        }
        if(!temp.key) return;

        if(temp.value < yRange.lower || temp.value > yRange.upper)
            this->yAxis->setRange(yRange.center(), abs(yRange.center()- temp.value)*2+4, Qt::AlignCenter);

        this->xAxis->setRange(temp.key + 1, xRange.size(), Qt::AlignRight);
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

void QTrendPlot::showContextMenu(QPoint position){
    QMenu *menu = new QMenu(this);
    QAction* temp;

    menu->setAttribute(Qt::WA_DeleteOnClose);

    menu->addAction((docked? "Undock from main window" : "Dock to main window"),this,SLOT(dock_undock()));

    menu->addAction((showLegend? "Hide legend" : "Show legend"),this,SLOT(toggleLegend()));

    temp = menu->addAction("Remove selected graphs",this,SLOT(removeSelectedGraphs()));
    if(!this->selectedGraphs().count()) temp->setEnabled(false);

    temp=menu->addAction("Add graphs (selected in tree)",this,SLOT(addSelectedGraphs()));
    if(!casuTree->selectedItems().count() && !groupTree->selectedItems().size()) temp->setEnabled(false);

    menu->addAction("Save to pdf",this,SLOT(saveToPDF()));

    menu->addAction("Close trend",this,SLOT(close()));

    menu->popup(this->mapToGlobal(position));
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
