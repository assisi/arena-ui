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
            if(QApplication::keyboardModifiers() == Qt::ControlModifier){
                if(itemAtMouse->isSelected())itemAtMouse->setSelected(0);
                else itemAtMouse->setSelected(1);
            }
            else{
                scene_->clearSelection();
                itemAtMouse->setSelected(1);
            }
        else if(QApplication::keyboardModifiers() != Qt::ControlModifier && !drag_true)
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
//-------------------------------------------------------------------------------
// Subclassed QGraphicsScene for a BUG [QTBUG-10138]
// http://www.qtcentre.org/threads/36953-QGraphicsItem-deselected-on-contextMenuEvent
QArenaScene::QArenaScene(QWidget *parent) : QGraphicsScene(parent){}

void QArenaScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
            event->accept();
            return;
        }
    if (event->button() == Qt::LeftButton) {
            event->accept();
            return;
        }
    //QGraphicsScene::mousePressEvent(event);
}
// -------------------------------------------------------------------------------


void ArenaUI::on_actionOpen_Arena_triggered()
{
    QProgressBar progress;
    progress.setMinimum(0);
    QString tempString = QFileDialog::getOpenFileName(this,tr("Open Arena configuration file"), settings->value("arenaFolder").toString(), tr("*.assisi;;*.arenaUI"));

    if(tempString.endsWith(".assisi")){
        assisiFile = tempString;
        assisiNode = YAML::LoadFile(assisiFile.toStdString());
        QString arenaFile = assisiFile.left(assisiFile.lastIndexOf('/')+1) + QString::fromStdString(assisiNode["arena"].as<std::string>());

        YAML::Node arenaNode = YAML::LoadFile(arenaFile.toStdString());
        QList<QString> layers;
        for(YAML::const_iterator it=arenaNode.begin(); it!=arenaNode.end(); it++) layers.append(QString::fromStdString(it->first.as<std::string>()));

        if(layers.size() > 1) arenaLayer = QInputDialog::getItem(this,tr("Select arena layer"),"",QStringList(layers));
        else arenaLayer = layers[0];

        arena_scene->clear();
        ui->casuTree->clear();

        progress.setMaximum(arenaNode[arenaLayer.toStdString()].size());
        progress.show();
        progress.move(ui->arenaSpace->mapToGlobal(QPoint(400-progress.width()/2,400-progress.height()/2)));

        for(YAML::const_iterator it=arenaNode[arenaLayer.toStdString()].begin(); it!=arenaNode[arenaLayer.toStdString()].end(); it++){
            QString name = QString::fromStdString(it->first.as<std::string>());
            int xpos = arenaNode[arenaLayer.toStdString()][name.toStdString()]["pose"]["x"].as<int>();
            int ypos = arenaNode[arenaLayer.toStdString()][name.toStdString()]["pose"]["y"].as<int>();
            int yaw = arenaNode[arenaLayer.toStdString()][name.toStdString()]["pose"]["yaw"].as<int>();

            QCasuTreeItem* tempTree = new QCasuTreeItem(ui->casuTree, name);

            ui->casuTree->addTopLevelItem(tempTree);

            QCasuSceneItem* tempItem = new QCasuSceneItem(this, xpos*10+400, -ypos*10+400, yaw, tempTree);

            arena_scene->addItem(tempItem);

            tempTree->setAddr(QString::fromStdString(arenaNode[arenaLayer.toStdString()][name.toStdString()]["sub_addr"].as<std::string>()),
                               QString::fromStdString(arenaNode[arenaLayer.toStdString()][name.toStdString()]["pub_addr"].as<std::string>()),
                               QString::fromStdString(arenaNode[arenaLayer.toStdString()][name.toStdString()]["msg_addr"].as<std::string>()));

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

void ArenaUI::toggleIR()
{
    settings->setValue("IR_on",!settings->value("IR_on").toBool());
}

void ArenaUI::toggleTemp()
{
    settings->setValue("temp_on",!settings->value("temp_on").toBool());
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

    menu->addAction(settings->value("IR_on").toBool() ? "Hide proximity sensors" : "Show proximity sensors",this,SLOT(toggleIR()));
    menu->addAction(settings->value("temp_on").toBool() ? "Hide temperature sensors" : "Show temperature sensors",this,SLOT(toggleTemp()));
    menu->addSeparator();
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

void ArenaUI::groupSave(QSettings *saveState, QList<QGraphicsItem *> items, QString groupName)
{
    saveState->beginGroup(groupName);
    for(int k=0; k < items.size() ; k++)
        if(!items[k]->childItems().size()) saveState->setValue(QString(k),((QCasuSceneItem*)items[k])->treeItem->casuName);
        else groupSave(saveState, items[k]->childItems(), QString(k));
    saveState->endGroup();
}

void ArenaUI::on_actionSave_triggered()
{
    QString saveFile = QFileDialog::getSaveFileName(this,tr("Save As"),settings->value("arenaFolder").toString(),tr("*.arenaUI"));

    if(!saveFile.size())return;
    if(!saveFile.endsWith(".arenaUI")) saveFile += ".arenaUI";
    if(QFile(saveFile).exists())QFile(saveFile).remove();

    QSettings saveState(saveFile,QSettings::IniFormat);
    saveState.setValue("assisiFile",assisiFile);
    saveState.setValue("arenaLayer",arenaLayer);

    //save CASU graphics scene

    QList<QGraphicsItem*> tempSelection = arena_scene->selectedItems(); // save active selection
    QPainterPath pp;
    pp.addRect(arena_scene->sceneRect()); // select all -- selectedItems() doesnt return group children which is a case with items()
    arena_scene->setSelectionArea(pp);

    groupSave(&saveState, arena_scene->selectedItems(),"CASU");

    pp = pp.subtracted(pp);
    pp.addRect(0,0,0,0);
    arena_scene->setSelectionArea(pp);
    foreach(QGraphicsItem* item, tempSelection) item->setSelected(true);

    saveState.beginGroup("trendGraphs");
    for(int k=0; k < trendTab->children().size() ; k++){
        saveState.beginGroup(QString(k));
        QTrendPlot* tempPlot = (QTrendPlot*) trendTab->children()[k];
        for(int i=0; i < tempPlot->graphCount(); i++)
            saveState.setValue(QString(i),tempPlot->connectionMap[tempPlot->graph(i)]->legendName);
        saveState.endGroup();
    }
    saveState.endGroup();
}
