#include "qgstreamerview.h"

QGstreamerView::QGstreamerView(QWidget *parent) :
    QGraphicsView(parent),
    docked(true)
{ 
    QGst::init();
    QGraphicsScene *gstrScene = new QGraphicsScene;
    this->setScene(gstrScene);

    QGst::Ui::GraphicsVideoSurface *surface = new QGst::Ui::GraphicsVideoSurface(this);
    widget = new QGst::Ui::GraphicsVideoWidget;
    widget->setSurface(surface);
    gstrScene->addItem(widget);

    source = QGst::ElementFactory::make("v4l2src");
    sink = surface->videoSink();
    pipeline = QGst::Pipeline::create();
    pipeline->add(source);
    pipeline->add(sink);
    if(source->link(sink) == false)qDebug("Link failed");

    pipeline->setState(QGst::StatePlaying);

    old_sizes.push_back(2);
    old_sizes.push_back(1);
    ((QSplitter *)(this->parentWidget()))->setSizes(old_sizes);

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,&QGstreamerView::customContextMenuRequested,this,&QGstreamerView::showContextMenu);
}

QGstreamerView::~QGstreamerView(){
}


void QGstreamerView::dock_undock(){
    if(docked){
        old_parent = this->parentWidget();
        old_sizes = ((QSplitter *) old_parent)->sizes();
        this->setParent(NULL, Qt::Window);
    }
    if(!docked){
        this->setParent(old_parent, Qt::Widget);
        ((QSplitter *)old_parent)->setSizes(old_sizes);
    }
    this->show();
    docked = !docked;
}

void QGstreamerView::resizeEvent(QResizeEvent *event){
    Q_UNUSED(event)
    this->fitInView(widget,Qt::KeepAspectRatio);
}

void QGstreamerView::showEvent(QShowEvent *event){
    Q_UNUSED(event)
    this->fitInView(widget,Qt::KeepAspectRatio);
}

void QGstreamerView::showContextMenu(QPoint pos){
    QMenu *menu = new QMenu(this);

    menu->setAttribute(Qt::WA_DeleteOnClose);

    menu->addAction((docked? "Undock from main window" : "Dock to main window"),this,SLOT(dock_undock()));

    menu->popup(this->mapToGlobal(pos));
}
