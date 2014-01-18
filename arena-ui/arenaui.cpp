#include <boost/lexical_cast.hpp>

#include <QThread>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsEllipseItem>

#include "arenaui.h"
#include "ui_arenaui.h"

#include "dev_msgs.pb.h"

using namespace nzmqt;
using namespace std;
using namespace AssisiMsg;
using namespace boost;

// -------------------------------------------------------------------------------

ArenaUI::ArenaUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ArenaUI),
    sub_addr_("tcp://127.0.0.1:5555"),
    pub_addr_("tcp://127.0.0.1:5556"),
    context_(0),
    pub_sock_(0),
    sub_sock_(0)
{
    ui->setupUi(this);
    arena_scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(arena_scene);

    MouseClickHandler* click_handler = new MouseClickHandler(arena_scene, this);
    arena_scene->installEventFilter(click_handler);

    context_ = createDefaultContext(this);
    context_->start();
    sub_sock_ = context_->createSocket(ZMQSocket::TYP_SUB, this);
    pub_sock_ = context_->createSocket(ZMQSocket::TYP_PUB, this);

    connect(sub_sock_, SIGNAL(messageReceived(const QList<QByteArray>&)),
            SLOT(messageReceived(const QList<QByteArray>&)));

    this->connect_();
}

// -------------------------------------------------------------------------------

ArenaUI::~ArenaUI()
{
    delete ui;
    delete context_;
    delete pub_sock_;
    delete sub_sock_;
}

// -------------------------------------------------------------------------------

void ArenaUI::connect_()
{
    pub_sock_->connectTo(pub_addr_);
    // Subscribe to everything!
    sub_sock_->subscribeTo("casu");
    sub_sock_->connectTo(sub_addr_);
}

// -------------------------------------------------------------------------------

/* protected slots */
void ArenaUI::messageReceived(const QList<QByteArray>& message)
{
    connected_ = true;

    string name(message.at(0).constData(), message.at(0).length());
    string device(message.at(1).constData(), message.at(1).length());
    string command(message.at(2).constData(), message.at(2).length());
    string data(message.at(3).constData(), message.at(3).length());

    if (device == "ir")
    {
        RangeArray ranges;
        ranges.ParseFromString(data);
        QTreeWidgetItem* ir = ui->tree_casu->topLevelItem(0)->child(0);
        for (int i = 0; i < ranges.range_size(); i++)
        {
            ir->child(i)->setData(1, Qt::DisplayRole,
                                  QVariant(lexical_cast<string>(ranges.range(i)).c_str()));
        }
    }

}

// -------------------------------------------------------------------------------

MouseClickHandler::MouseClickHandler(QGraphicsScene* scene, QObject *parent) :
    QObject(parent),
    scene_(scene),
    pen_(Qt::red),
    mark_(0)
{
    pen_.setWidth(2);
}

// -------------------------------------------------------------------------------


bool MouseClickHandler::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::GraphicsSceneMousePress)
    {
        QGraphicsSceneMouseEvent* mouse_click = static_cast<QGraphicsSceneMouseEvent *>(event);
        if (mark_ == 0)
        {
            //QPointF point = mouse_click->buttonDownPos(Qt::LeftButton);
            QPointF point = mouse_click->scenePos();
            mark_ = scene_->addEllipse(point.x(), point.y(), 20, 20, pen_);
        }
        else
        {
            //mark_->setPos(mouse_click->buttonDownPos(Qt::LeftButton));
            mark_->setPos(mouse_click->scenePos());
        }
        return true;
    }
    else
    {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}

// -------------------------------------------------------------------------------
