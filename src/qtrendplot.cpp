#include "qtrendplot.h"

using namespace zmqData;

const std::vector<QString> graphColors = {"bisque", "blanchedalmond", "blue", "blueviolet", "brown",
                                          "burlywood", "cadetblue", "chartreuse", "chocolate", "coral",
                                          "cornflowerblue", "crimson", "cyan", "darkblue", "darkcyan",
                                          "darkgoldenrod", "darkgray", "darkgreen", "darkgrey", "darkkhaki",
                                          "darkmagenta", "darkolivegreen", "darkorange", "darkorchid",
                                          "darkred", "darksalmon", "darkseagreen", "darkslateblue",
                                          "darkslategray", "darkslategrey", "darkturquoise", "darkviolet",
                                          "deeppink", "deepskyblue", "dimgray", "dimgrey", "dodgerblue",
                                          "firebrick", "forestgreen", "fuchsia", "gainsboro", "gold",
                                          "goldenrod", "green", "greenyellow", "hotpink", "indianred",
                                          "indigo", "khaki", "lawngreen", "lightblue", "lightcoral",
                                          "lightgreen", "lightgrey", "lightpink", "lightsalmon",
                                          "lightseagreen", "lightskyblue", "lightslategray","lightslategrey",
                                          "lightsteelblue", "lime", "limegreen", "magenta", "maroon",
                                          "mediumaquamarine", "mediumblue", "mediumorchid", "mediumpurple",
                                          "mediumseagreen", "mediumslateblue", "mediumspringgreen",
                                          "mediumturquoise", "mediumvioletred", "midnightblue", "mintcream",
                                          "mistyrose", "moccasin", "navajowhite", "navy", "olive", "olivedrab",
                                          "orange", "orangered", "orchid", "palegoldenrod", "palegreen",
                                          "paleturquoise", "palevioletred", "papayawhip", "peachpuff", "peru",
                                          "pink", "plum", "powderblue", "purple", "red", "rosybrown",
                                          "royalblue", "saddlebrown", "salmon", "sandybrown", "seagreen",
                                          "seashell", "sienna", "silver", "skyblue", "slateblue", "slategray",
                                          "slategrey", "springgreen", "steelblue", "tan", "teal", "thistle",
                                          "tomato", "turquoise", "violet", "wheat", "yellow", "yellowgreen"};

QTrendPlot::QTrendPlot(QTreeWidget* tree1,QTreeWidget* tree2 , QWidget *parent) :
    QCustomPlot(parent),
    casuTree(tree1),
    groupTree(tree2),
    autoPosition(true),
    showLegend(true),
    docked(true)
{
   QSizePolicy customPolicy(sizePolicy());
   customPolicy.setHeightForWidth(true);
   setSizePolicy(customPolicy);

   QSharedPointer<QCPAxisTickerDateTime> dateTimeTicker(new QCPAxisTickerDateTime);
   dateTimeTicker->setDateTimeFormat("hh:mm:ss");
   xAxis->setTicker(dateTimeTicker);

   setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectLegend | QCP::iSelectPlottables | QCP::iMultiSelect);

   setMultiSelectModifier(Qt::ControlModifier);

   setAutoAddPlottableToLegend(true);
   legend->setVisible(showLegend);
   legend->setSelectableParts(QCPLegend::spItems);

   setContextMenuPolicy(Qt::CustomContextMenu);

   connect(this,&QTrendPlot::mouseDoubleClick, [&](){
       autoPosition = true;
   });
   connect(this,&QTrendPlot::mouseMove, [&](QMouseEvent *event){
       if(event->buttons() == Qt::LeftButton)autoPosition = false;
   });
   connect(this,&QTrendPlot::mouseWheel, [&](QWheelEvent *event){
       if(event->modifiers() & Qt::ShiftModifier){
           axisRect()->setRangeZoom(Qt::Horizontal);
       } else if(event->modifiers() & Qt::ControlModifier){
                axisRect()->setRangeZoom(Qt::Vertical);
            }
       else{
           axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
       }
   });
   connect(this,&QTrendPlot::selectionChangedByUser, [&](){
       for (int k=0; k<graphCount(); k++){
           auto item = legend->itemWithPlottable(graph(k));
           if (item->selected() || graph(k)->selected()){
             item->setSelected(true);
             graph(k)->setSelection(QCPDataSelection(QCPDataRange(0,graph(k)->dataCount())));
           }
         }
   });
   connect(this,&QTrendPlot::customContextMenuRequested,this,&QTrendPlot::showContextMenu);
   connect(this,&QTrendPlot::beforeReplot,this,&QTrendPlot::prettyPlot);
}

void QTrendPlot::addGraph(QSharedPointer<zmqBuffer> buffer){
    for(int k=0; k<graphCount(); k++){
        if (!QString::compare(graph(k)->name(), buffer->getLegendName())) return;
    }
    QCustomPlot::addGraph();
    graph()->setData(buffer);
    graph()->setName(buffer->getLegendName());
    graph()->setPen(QPen(Qt::black));
    graph()->setSelectable(QCP::stWhole);

    while(1){
        bool color_used = false;
        QString color = graphColors[std::rand() % graphColors.size()];
        for(int i = 0; i < graphCount() ; i++){
            if(graph(i)->pen().color() == color) color_used = true;
        }
        if(!color_used){
            graph()->setPen(QPen(color));
            break;
        }
    }
    connect(buffer.data(), &zmqBuffer::updatePlot, [&](){ replot(QCustomPlot::rpQueuedReplot); });
    m_connectionMap.insert(graph(), buffer);
}

void QTrendPlot::addGraphList(QList<QSharedPointer<zmqBuffer> > &bufferList)
{    
    bool new_trend = true;
    if(graphCount()) new_trend = false;

    // even though bufferList is QList by reference, these changes are positive on whole QList
    bufferList = bufferList.toSet().toList(); //remove duplicates
    qSort(bufferList.begin(),bufferList.end(),[](QSharedPointer<zmqBuffer> b1, QSharedPointer<zmqBuffer> b2){
        return QString::compare(b1->getLegendName(), b2->getLegendName()) < 0;
    }); //sort by legend name

    for(auto& buffer : bufferList){
        addGraph(buffer);
    }

    if(new_trend){
        rescaleAxes();
        if(yAxis->range().size() < 5)yAxis->setRange(yAxis->range().center(), 5, Qt::AlignCenter);
        if(graph()->data()->isEmpty())xAxis->setRange(QTime(0,0,0).msecsTo(QTime::currentTime()) /1000, 60, Qt::AlignRight);
        else xAxis->setRange(std::prev(graph()->data()->constEnd())->key, 60, Qt::AlignRight);
    }
}

void QTrendPlot::removeGraph(QCPGraph *graph){
    QCustomPlot::removeGraph(graph);
    disconnect(m_connectionMap[graph].data(),0,this,0);
    m_connectionMap.remove(graph);
}

QSharedPointer <zmqData::zmqBuffer> QTrendPlot::link(QCPGraph *graph)
{
    return m_connectionMap[graph];
}

void QTrendPlot::addSelectedGraphs(){

    QList<QSharedPointer <zmqData::zmqBuffer> > bufferList;

    for(int k=0; k < casuTree->topLevelItemCount(); k++)
        bufferList.append(tCast(casuTree->topLevelItem(k))->getBuffers());
    for(int k=0; k < groupTree->topLevelItemCount(); k++)
        bufferList.append(tCast(groupTree->topLevelItem(k))->getBuffers());

    addGraphList(bufferList);

    replot(QCustomPlot::rpQueuedReplot);
}

void QTrendPlot::prettyPlot()
{
    //it is called before replot() to tidy everything
    double labelAngle = 45 - 30*(size().width() - 340)/ size().width(); //calculate angle so tick labels are readable
    xAxis->setTickLabelRotation(labelAngle > 30 ? labelAngle : 0); //angle is not needed when it is less than 30°

    if(autoPosition){
        auto yRange = yAxis->range();
        auto xRange = xAxis->range();

        QCPGraphData temp;
        temp.key = 0;

        for(int k = 0; k < graphCount();k++){
            QSharedPointer<QCPGraphDataContainer> tempMap = graph(k)->data();
            if(tempMap->isEmpty()) continue;
            const QCPGraphData* tempData = std::prev(tempMap->constEnd());
            if(k==0 || temp.key < tempData->key) temp = *tempData;
        }
        if(!temp.key) return;

        if(temp.value < yRange.lower || temp.value > yRange.upper){
            yAxis->setRange(yRange.center(), abs(yRange.center()- temp.value)*2+4, Qt::AlignCenter);
        }

        xAxis->setRange(temp.key + 1, xRange.size(), Qt::AlignRight);
    }
}


void QTrendPlot::showContextMenu(QPoint position){
    auto menu = new QMenu(this);
    QAction* tempAction;

    menu->setAttribute(Qt::WA_DeleteOnClose);

    // FIXME: Qt 5.6 QMenu::addAction accepts Qt5 style connect (possible lambda expressions)
    tempAction = menu->addAction((docked? "Undock from main window" : "Dock to main window"));
    connect(tempAction, &QAction::triggered, [&](){
        docked = !docked;
        setParent(parentWidget(), docked ? Qt::Widget : Qt::Window);
        show();
    });

    tempAction = menu->addAction((showLegend? "Hide legend" : "Show legend"));
    connect(tempAction, &QAction::triggered, [&](){
        showLegend = !showLegend;
        legend->setVisible(showLegend);
        replot(QCustomPlot::rpQueuedReplot);
    });

    tempAction = menu->addAction("Remove selected graphs");
    if(!selectedGraphs().count()) tempAction->setEnabled(false);
    connect(tempAction, &QAction::triggered, [&](){
        while(selectedGraphs().count())
            removeGraph(selectedGraphs().first());
    });

    tempAction = menu->addAction("Add graphs (selected in tree)");
    connect(tempAction, &QAction::triggered, this, &QTrendPlot::addSelectedGraphs);
    if(!casuTree->selectedItems().count() && !groupTree->selectedItems().size()) tempAction->setEnabled(false);

    tempAction = menu->addAction("Save to pdf");
    connect(tempAction, &QAction::triggered, [&](){
        auto path = QFileDialog::getSaveFileName(this,tr("Export trend graph as PDF"),QString(), tr("*.pdf"));
        if(!path.endsWith(".pdf",Qt::CaseInsensitive)) path+=".pdf";
        if(path.size()) savePdf(path);
    });

    tempAction = menu->addAction("Close trend");
    connect(tempAction, &QAction::triggered, this, &QTrendPlot::close);

    menu->popup(mapToGlobal(position));
}
