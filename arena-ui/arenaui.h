#ifndef ARENAUI_H
#define ARENAUI_H

#include <QMainWindow>

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
    Ui::ArenaUI *ui;

    QString sub_addr_;
    QString pub_addr_;
    nzmqt::ZMQContext* context_;
    nzmqt::ZMQSocket* pub_sock_;
    nzmqt::ZMQSocket* sub_sock_;
};

#endif // ARENAUI_H
