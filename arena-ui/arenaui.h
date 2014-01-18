#ifndef ARENAUI_H
#define ARENAUI_H

#include <QMainWindow>
#include <QGraphicsScene>

#include <nzmqt.hpp>

namespace Ui {
class ArenaUI;
}

class ArenaUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit ArenaUI(QWidget *parent = 0);
    ~ArenaUI();

private:

    // Connect the publisher and subscriber
    void connect_();

    Ui::ArenaUI *ui;
    QGraphicsScene *arena_scene;
    QGraphicsEllipseItem *ellipse;

    QString sub_addr_;
    QString pub_addr_;
    nzmqt::ZMQContext* context_;
    nzmqt::ZMQSocket* pub_sock_;
    nzmqt::ZMQSocket* sub_sock_;
    bool connected_;

protected slots:
    void messageReceived(const QList<QByteArray>& message);
};


class MouseClickHandler : public QObject
{
    Q_OBJECT

public:
    MouseClickHandler(QObject* parent = 0);

protected:
    bool eventFilter(QObject* obj, QEvent* event);
};

#endif // ARENAUI_H
