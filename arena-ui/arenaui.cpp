#include <boost/lexical_cast.hpp>

#include <QThread>
#include <QGraphicsSceneMouseEvent>
#include <QFileDialog>
#include <QMessageBox>

#include "arenaui.h"
#include "ui_arenaui.h"


using namespace nzmqt;
using namespace std;
using namespace AssisiMsg;
using namespace boost;


// -------------------------------------------------------------------------------

ArenaUI::ArenaUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ArenaUI)
{
    loadConfig();
    ui->setupUi(this);
    ui->actionToggleLog->setChecked(settings->value("log_on").toBool());

    //CASU TREE TAB
    ui->casuTree->addAction(ui->actionPlot_selected_in_same_trend);
    ui->casuTree->addAction(ui->actionPlot_selected_in_different_trends);
    ui->casuTree->header()->setSortIndicator(0,Qt::AscendingOrder);

    //TREND TAB SCROLLABLE LAYOUT
    ui->tab_trend->setLayout(new QVBoxLayout);
    QWidget* tempWidget = new QWidget;
    QScrollArea* tempScroll = new QScrollArea;

    tempScroll->setWidget(tempWidget);
    tempScroll->setWidgetResizable(true);
    trendTab = new QVBoxLayout(tempWidget);

    ui->tab_trend->layout()->addWidget(tempScroll);

    //GRAPHICS SCENE
    arena_scene = new QArenaScene(this);
    arena_scene->setSceneRect(0,0,800,800);

    ui->arenaSpace->setScene(arena_scene);
    ui->arenaSpace->setDragMode(QGraphicsView::RubberBandDrag);

    MouseClickHandler* click_handler = new MouseClickHandler(arena_scene, this);
    arena_scene->installEventFilter(click_handler);

    connect(ui->arenaSpace,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenu(QPoint)));
}

ArenaUI::~ArenaUI()
{
    delete ui;
}

// -------------------------------------------------------------------------------

MouseClickHandler::MouseClickHandler(QGraphicsScene* scene, QObject *parent) :
    QObject(parent),
    scene_(scene){}

bool MouseClickHandler::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::GraphicsSceneMouseMove){
        drag_true = true;
    }
    if (event->type() == QEvent::GraphicsSceneMouseRelease)
    {
        QGraphicsSceneMouseEvent* mouse_click = static_cast<QGraphicsSceneMouseEvent *>(event);

        QGraphicsItem * itemAtMouse= scene_->itemAt(mouse_click->scenePos().x(),mouse_click->scenePos().y(), QTransform());

        if (itemAtMouse && !drag_true)
            if(QApplication::keyboardModifiers() == Qt::CTRL){
                if(itemAtMouse->isSelected())itemAtMouse->setSelected(0);
                else itemAtMouse->setSelected(1);
            }
            else{
                scene_->clearSelection();
                itemAtMouse->setSelected(1);
            }
        else if(QApplication::keyboardModifiers() != Qt::CTRL && !drag_true)
            scene_->clearSelection();


        drag_true = false;

        return true;
    }
    else
    {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}

// -------------------------------------------------------------------------------


void ArenaUI::on_actionOpen_Arena_triggered()
{
    QProgressBar progress;
    progress.setMinimum(0);

    arenaFile = QFileDialog::getOpenFileName(this,tr("Open Arena configuration file"), settings->value("arenaFolder").toString(), tr("*.arena"));
    if(arenaFile.size()){
        arena_scene->clear();
        ui->casuTree->clear();
        YAML::Node arena_config = YAML::LoadFile(arenaFile.toStdString());
        progress.setMaximum(arena_config["beearena"].size());
        progress.show();
        progress.move(ui->arenaSpace->mapToGlobal(QPoint(400-progress.width()/2,400-progress.height()/2)));

        for(YAML::const_iterator it=arena_config["beearena"].begin(); it!=arena_config["beearena"].end(); it++){
            QString name = QString::fromStdString(it->first.as<std::string>());
            int xpos = arena_config["beearena"][name.toStdString()]["pose"]["x"].as<int>();
            int ypos = arena_config["beearena"][name.toStdString()]["pose"]["y"].as<int>();
            int yaw = arena_config["beearena"][name.toStdString()]["pose"]["yaw"].as<int>();

            QCasuTreeItem* tempTree = new QCasuTreeItem(ui->casuTree, name);

            ui->casuTree->addTopLevelItem(tempTree);

            QCasuSceneItem* tempItem = new QCasuSceneItem(this, xpos*10+400, -ypos*10+400, yaw, tempTree);

            arena_scene->addItem(tempItem);

            tempTree->setAddr(QString::fromStdString(arena_config["beearena"][name.toStdString()]["sub_addr"].as<std::string>()),
                               QString::fromStdString(arena_config["beearena"][name.toStdString()]["pub_addr"].as<std::string>()),
                               QString::fromStdString(arena_config["beearena"][name.toStdString()]["msg_addr"].as<std::string>()));

            progress.setValue(progress.value()+1);
            QApplication::processEvents();
        }

    }
}

void ArenaUI::on_actionGroup_triggered()
{
    QList<QGraphicsItem *> temp_itemList= arena_scene->selectedItems();
    arena_scene->clearSelection();
    QGraphicsItemGroup *temp_group = arena_scene->createItemGroup(temp_itemList);
    temp_group->setFlag(QGraphicsItem::ItemIsSelectable, true);
    temp_group->setFlag(QGraphicsItem::ItemIsFocusable, false);
    temp_group->setSelected(1);
    temp_group->setZValue(-1);
}


void ArenaUI::on_actionUngroup_triggered()
{
    QList<QGraphicsItem *> temp_itemList= arena_scene->selectedItems();
    for(int k=0;k<temp_itemList.size();k++)
        if(temp_itemList[k]->childItems().size()){
            QList<QGraphicsItem *> tempChildList = temp_itemList[k]->childItems();
            if(!temp_itemList[k]->childItems().size())temp_itemList[k]->setZValue(1);
            arena_scene->destroyItemGroup((QGraphicsItemGroup*)temp_itemList[k]);
            for(int i=0;i<tempChildList.size();i++){;
                tempChildList[i]->setSelected(0);
                tempChildList[i]->setSelected(1);
            }
        }
}

void ArenaUI::on_actionConnect_triggered()
{
    bool error = false;
    if (arena_scene->selectedItems().size() != 1) error = true; //Check if excactly one object is selected
    if (arena_scene->selectedItems().size() == 1)
        if(arena_scene->selectedItems()[0]->childItems().size()) error = true; // Check if object is single casu (no children)
    if(error){
        QMessageBox msgBox;
        msgBox.setWindowTitle("ERROR");
        msgBox.setText("Please select one CASU.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return;
    }
    QCasuSceneItem* temp = (QCasuSceneItem*)arena_scene->selectedItems()[0];
    QDialogConnect* addrDiag = new QDialogConnect(temp->treeItem->sub_addr,temp->treeItem->pub_addr,temp->treeItem->msg_addr);
    if(addrDiag->exec()){
        temp->treeItem->setAddr(addrDiag->sub_addr->text(), addrDiag->pub_addr->text(),addrDiag->msg_addr->text());
    }
}

void ArenaUI::on_actionToggleLog_triggered()
{
    ui->actionToggleLog->setChecked(settings->value("log_on").toBool());
    QString question = QString("Are you sure you want to turn ") + (settings->value("log_on").toBool() ? QString("OFF") : QString("ON")) + QString(" logging?");
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Toggle Log", question , QMessageBox::Yes|QMessageBox::No);
    if(reply == QMessageBox::Yes)settings->setValue("log_on", !settings->value("log_on").toBool());
    ui->actionToggleLog->setChecked(settings->value("log_on").toBool());
}

void ArenaUI::on_actionPlot_selected_in_same_trend_triggered()
{
    QTrendPlot* tempWidget = new QTrendPlot(ui->casuTree);
    trendTab->addWidget(tempWidget);

    tempWidget->addSelectedGraphs();
}

void ArenaUI::on_actionSettings_triggered()
{
    QDialogSettings* settingsDiag = new QDialogSettings();
    settingsDiag->exec();
}

void ArenaUI::customContextMenu(QPoint pos)
{
    QMenu* menu = new QMenu(); //no parent because it inherits background image from QGraphicsView
    QAction* temp;

    menu->setAttribute(Qt::WA_DeleteOnClose);

    bool error_single = false;
    if (arena_scene->selectedItems().size() != 1) error_single = true; //Check if excactly one object is selected
    if (arena_scene->selectedItems().size() == 1)
        if(arena_scene->selectedItems()[0]->childItems().size()) error_single = true; // Check if object is single casu (no children)

    bool error_selected = !arena_scene->selectedItems().size();

    //menu->addAction("Open arena file",this,SLOT(on_actionOpen_Arena_triggered()));
    //menu->addAction("Toggle logging",this,SLOT(on_actionToggleLog_triggered()));
    //menu->addSeparator();
    temp = menu->addAction("Group selected",this,SLOT(on_actionGroup_triggered()));
    if(error_selected) temp->setEnabled(false);
    temp = menu->addAction("Ungroup selected",this,SLOT(on_actionUngroup_triggered()));
    if(error_selected) temp->setEnabled(false);
    menu->addSeparator();
    temp = menu->addAction("Set connection",this,SLOT(on_actionConnect_triggered()));
    if(error_single) temp->setEnabled(false);

    QMenu* sendMenu = new QMenu("Setpoint");
    if(error_selected) sendMenu->setEnabled(false);
    menu->addMenu(sendMenu);

    QSignalMapper* signalMapper = new QSignalMapper(menu);

    temp = sendMenu->addAction("Temperature",signalMapper,SLOT(map()));
    signalMapper->setMapping(temp,"Temperature");
    temp = sendMenu->addAction("Vibration",signalMapper,SLOT(map()));
    signalMapper->setMapping(temp,"Vibration");
    temp = sendMenu->addAction("Airflow",signalMapper,SLOT(map()));
    signalMapper->setMapping(temp,"Airflow");
    temp = sendMenu->addAction("LED",signalMapper,SLOT(map()));
    signalMapper->setMapping(temp,"LED");

    connect(signalMapper,SIGNAL(mapped(QString)),this,SLOT(sendSetpoint(QString)));

    menu->popup(ui->arenaSpace->mapToGlobal(pos));
}

void ArenaUI::sendSetpoint(QString actuator)
{
    QDialogSetpoint* dialog = new QDialogSetpoint(actuator);
    if(dialog->exec())
        for(int k = 0; k < arena_scene->selectedItems().count(); k++)
            if(arena_scene->selectedItems()[k]->childItems().size())
                groupSendSetpoint(arena_scene->selectedItems()[k],dialog->message);
            else
                ((QCasuSceneItem*)arena_scene->selectedItems()[k])->treeItem->sendSetpoint(dialog->message);
}

void ArenaUI::groupSendSetpoint(QGraphicsItem *group, QList<QByteArray> message)
{
    for(int k = 0; k < group->childItems().count(); k++)
        if(group->childItems()[k]->childItems().size())
            groupSendSetpoint(group->childItems()[k],message);
        else
            ((QCasuSceneItem*)group->childItems()[k])->treeItem->sendSetpoint(message);
}


QArenaScene::QArenaScene(QWidget *parent) : QGraphicsScene(parent){}

void QArenaScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
            event->accept();
            return;
        }
        QGraphicsScene::mousePressEvent(event);
}
