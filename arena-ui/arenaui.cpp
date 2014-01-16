#include "arenaui.h"
#include "ui_arenaui.h"

#include <QThread>

#include "sim_msgs.pb.h"

using namespace nzmqt;
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

    context_ = createDefaultContext(this);
    context_->start();
    sub_sock_ = context_->createSocket(ZMQSocket::TYP_SUB, this);
    pub_sock_ = context_->createSocket(ZMQSocket::TYP_PUB, this);

    pub_sock_->connectTo(pub_addr_);

    QList<QByteArray> msg;
    msg.append(QByteArray("sim"));
    msg.append(QByteArray("spawn"));
    msg.append(QByteArray("Casu"));

    AssisiMsg::Spawn spawn_msg;
    spawn_msg.set_type("Casu");
    spawn_msg.set_name("casu1");
    spawn_msg.mutable_pose()->mutable_position()->set_x(0);
    spawn_msg.mutable_pose()->mutable_position()->set_y(0);
    spawn_msg.mutable_pose()->mutable_orientation()->set_z(0);
    std::string msg_str;
    spawn_msg.SerializeToString(&msg_str);
    msg.append(QByteArray(msg_str.c_str(), msg_str.length()));
    QThread::sleep(2);
    pub_sock_->sendMessage(msg);

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
