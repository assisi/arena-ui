#include <QThread>
#include <QGraphicsSceneMouseEvent>
#include <QFileDialog>
#include <QMessageBox>

#include "arenaui.h"
#include "ui_arenaui.h"
#include "qdialogsetpointvibepattern.h"

using namespace zmqData;

// -------------------------------------------------------------------------------

ArenaUI::ArenaUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ArenaUI)
{
    QLocale::setDefault(QLocale::C); //override for localization to use C locale

    loadConfig();
    ui->setupUi(this);
    ui->actionToggleLog->setChecked(g_settings->value("log_on").toBool());

    // CASU TREE TAB
    ui->casuTree->addAction(ui->actionPlot_selected_in_same_trend);
    ui->casuTree->addAction(ui->actionPlot_selected_in_different_trends);
    ui->casuTree->header()->setSortIndicator(0,Qt::AscendingOrder);

    ui->groupTree->addAction(ui->actionPlot_selected_in_same_trend);
    ui->groupTree->addAction(ui->actionPlot_selected_in_different_trends);

    auto tempLambda = [&](){
        if(ui->casuTree->selectedItems().size() || ui->groupTree->selectedItems().size()){
            ui->actionPlot_selected_in_same_trend->setEnabled(true);
            ui->actionPlot_selected_in_different_trends->setEnabled(true);
        } else {
            ui->actionPlot_selected_in_same_trend->setEnabled(false);
            ui->actionPlot_selected_in_different_trends->setEnabled(false);
        }
    };
    connect(ui->casuTree, &QTreeWidget::itemSelectionChanged, tempLambda);
    connect(ui->groupTree, &QTreeWidget::itemSelectionChanged, tempLambda);
    connect(ui->casuTree, &QTreeWidget::itemSelectionChanged, [&](){
        if (!ui->casuTree->selectedItems().size()) return;
        if (!(QApplication::keyboardModifiers() & Qt::ControlModifier)) ui->groupTree->clearSelection();
    });
    connect(ui->groupTree, &QTreeWidget::itemSelectionChanged, [&](){
        if (!ui->groupTree->selectedItems().size()) return;
        if (!(QApplication::keyboardModifiers() & Qt::ControlModifier)) ui->casuTree->clearSelection();
    });


    // TREND TAB SCROLLABLE LAYOUT

    auto tempWidget = new QWidget;
    auto tempScroll = new QScrollArea;

    tempScroll->setWidget(tempWidget);
    tempScroll->setWidgetResizable(true);
    m_trendTab = new QVBoxLayout(tempWidget);

    ui->tabTrend->setLayout(new QVBoxLayout);
    ui->tabTrend->layout()->addWidget(tempScroll);

    // DEPLOYMENT TAB
    // - set custom flow layout
    for(auto& widget : ui->deployButtons->children())
        ui->deployButtons->layout()->removeWidget(qobject_cast<QWidget *>(widget));
    auto tempLayout = new FlowLayout;
    delete ui->deployButtons->layout();
    ui->deployButtons->setLayout(tempLayout);
    auto tempList = ui->deployButtons->children();
    tempList.removeLast();
    qSort(tempList.begin(),tempList.end(),[](QObject *obj1, QObject *obj2){
        // objects have names "_#objectName" where # is wanted position
        return obj1->objectName().at(1) < obj2->objectName().at(1);
    });
    for(auto& widget : tempList)
        ui->deployButtons->layout()->addWidget(qobject_cast<QWidget *>(widget));

    // - set deploy widget in scroll area
    ui->scrollArea->setWidget(ui->deployWidget);
    connect(ui->scrollArea->verticalScrollBar(), &QScrollBar::rangeChanged,[&](int min, int max){
        Q_UNUSED(min)
        ui->scrollArea->verticalScrollBar()->setValue(max);
    });

    // GRAPHICS SCENE
    m_arenaScene = new QArenaScene(this);
    m_arenaScene->setSceneRect(0,0,800,800);
    ui->arenaSpace->setScene(m_arenaScene);
    ui->arenaSpace->setDragMode(QGraphicsView::RubberBandDrag);
    new QGraphicsViewZoom(ui->arenaSpace);

    auto click_handler = new MouseClickHandler(m_arenaScene, this);
    m_arenaScene->installEventFilter(click_handler);

    connect(ui->arenaSpace, &QGraphicsView::customContextMenuRequested, this, &ArenaUI::customContextMenu);

    // - define setpoint actions for context menu
    {
        m_signalMapper = new QSignalMapper(this);
        connect(m_signalMapper, static_cast<void (QSignalMapper::*)(const QString &)>(&QSignalMapper::mapped), this,[&](QString actuator){
            if(m_arenaScene->selectedItems().size()){
                QDialog* dialog;
                if(actuator == "VibrationPattern")
                    dialog = new QDialogSetpointVibePattern(this, m_arenaScene->selectedItems());
                else
                    dialog = new QDialogSetpoint(this, actuator, m_arenaScene->selectedItems());
                dialog->exec();
            }
        });

        auto temp_lambda = [&](QString name, Qt::Key key){
            QAction* tempAction = new QAction(name, this);
            connect(tempAction, SIGNAL(triggered(bool)), m_signalMapper, SLOT(map()));
            m_signalMapper->setMapping(tempAction, name);
            tempAction->setShortcutContext(Qt::ApplicationShortcut);
            tempAction->setShortcut(QKeySequence(key));
            addAction(tempAction);
            m_setpointActions.append(tempAction);
        };
        temp_lambda("Temperature", Qt::Key_F1);
        temp_lambda("Vibration", Qt::Key_F2);
        temp_lambda("Airflow", Qt::Key_F3);
        temp_lambda("IR Proximity", Qt::Key_F4);
        temp_lambda("LED", Qt::Key_F5);
        temp_lambda("VibrationPattern", Qt::Key_F6);
    }

    // - 30fps update
    m_sceneUpdate = new QTimer(this);
    m_sceneUpdate->start(33);
    // NOTE: QGraphicsScene::update() has default value
    connect(m_sceneUpdate, &QTimer::timeout, [&](){ m_arenaScene->update(); });
}

ArenaUI::~ArenaUI()
{
    delete ui;
}

void ArenaUI::sortGraphicsScene()
{
    auto tempSelection = m_arenaScene->selectedItems(); // save active selection
    QPainterPath pp;
    pp.addRect(m_arenaScene->sceneRect()); // select all -- selectedItems() doesnt return group children which is a case with items()
    m_arenaScene->setSelectionArea(pp);

    for(int k = 0; k < m_arenaScene->selectedItems().size(); k++)
        m_arenaScene->selectedItems()[k]->setZValue(k+1);

    for(int k = 0; k+1 < m_arenaScene->selectedItems().size(); k++)
        for(int i = k+1; i < m_arenaScene->selectedItems().size(); i++){
            auto item1 = sCast(m_arenaScene->selectedItems()[k]);
            auto item2 = sCast(m_arenaScene->selectedItems()[i]);

            auto path1 = std::move(item1->completeShape());
            auto path2 = std::move(item2->completeShape());

            if(path1.intersects(path2)){
                int z1 = item1->zValue();
                int z2 = item2->zValue();
                if(z1 > z2) std::swap(z1,z2);

                if(path1.intersects(item2->shape())){
                    item1->setZValue(z1);
                    item2->setZValue(z2);
                }
                if(path2.intersects(item1->shape())){
                    item1->setZValue(z2);
                    item2->setZValue(z1);
                }
            }
        }

    pp = pp.subtracted(pp);
    pp.addRect(0,0,0,0);
    m_arenaScene->setSelectionArea(pp);
    for(auto& item : tempSelection) item->setSelected(true); // after saving items and groups, return selection as was before

}

// -------------------------------------------------------------------------------

MouseClickHandler::MouseClickHandler(QGraphicsScene* scene, QObject *parent) :
    QObject(parent),
    scene_(scene),
    dragTrue(false){}

bool MouseClickHandler::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::GraphicsSceneMousePress){
        if(((QGraphicsSceneMouseEvent*)event)->button() == Qt::RightButton){
            return QObject::eventFilter(obj, event);
        }
        if(QApplication::keyboardModifiers() == Qt::ControlModifier){
            selectedList = scene_->selectedItems();
        } else selectedList.clear();

        return QObject::eventFilter(obj, event);;
    }
    else
    if (event->type() == QEvent::GraphicsSceneMouseMove){
        for(auto& item : selectedList) item->setSelected(true);
        dragTrue = true;
        scene_->update();

        return QObject::eventFilter(obj, event);;
    }
    else
    if (event->type() == QEvent::GraphicsSceneMouseRelease)
    {
        for(auto& item : selectedList) item->setSelected(true);

        auto mouse_click = static_cast<QGraphicsSceneMouseEvent *>(event);
        auto itemAtMouse= scene_->itemAt(mouse_click->scenePos().x(),mouse_click->scenePos().y(), QTransform());

        if (itemAtMouse && !dragTrue){
            if(QApplication::keyboardModifiers() == Qt::ControlModifier){
                if(itemAtMouse->isSelected()){
                        itemAtMouse->setSelected(0);
                } else {
                itemAtMouse->setSelected(1);
            }
            } else {
                scene_->clearSelection();
                itemAtMouse->setSelected(1);
            }
        } else if(QApplication::keyboardModifiers() != Qt::ControlModifier && !dragTrue){
                scene_->clearSelection();
            }

        dragTrue = false;
        scene_->update();

        return QObject::eventFilter(obj, event);;
    }
    else
    {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}

// -------------------------------------------------------------------------------

void ArenaUI::on_actionOpenArena_triggered()
{
    QProgressBar progress;
    progress.setMinimum(0);
    auto loadFile = QFileDialog::getOpenFileName(this,tr("Open Arena configuration file"), g_settings->value("arenaFolder").toString(), tr("All(*.arenaUI *assisi);;Project(*.assisi);;Session(*.arenaUI)"));
    if(!loadFile.size()) return;

    while(m_trendTab->count()){
        dynamic_cast<QCustomPlot *> (m_trendTab->itemAt(0)->widget())->close();
        m_trendTab->removeWidget(m_trendTab->itemAt(0)->widget());
    }

    m_arenaScene->clear();
    ui->casuTree->clear();
    ui->groupTree->clear();
    /*Create new selectionTreeItem*/{
        auto tempTreeItem = new QSelectionTreeItem(m_arenaScene);
        ui->groupTree->addTopLevelItem(tempTreeItem);
        m_arenaScene->setTreeItem(tempTreeItem);
    }

// FIXME: github issue #39
    if(loadFile.endsWith(".assisi")){
        g_assisiFile.name = loadFile;
        m_assisiNode = YAML::LoadFile(g_assisiFile.name.toStdString());

        ui->arenaLabel->setText(QString::fromStdString(m_assisiNode["arena"].as<std::string>()));
        ui->neighLabel->setText(QString::fromStdString(m_assisiNode["dep"].as<std::string>()));
        ui->deployLabel->setText(QString::fromStdString(m_assisiNode["nbg"].as<std::string>()));

        ui->deployWidget->setWorkingDirectory(g_assisiFile.name.left(g_assisiFile.name.lastIndexOf('/')));

        g_assisiFile.arenaFile = g_assisiFile.name.left(g_assisiFile.name.lastIndexOf('/')+1) + QString::fromStdString(m_assisiNode["arena"].as<std::string>());
        g_assisiFile.depFile = g_assisiFile.name.left(g_assisiFile.name.lastIndexOf('/')+1) + QString::fromStdString(m_assisiNode["dep"].as<std::string>());
        g_assisiFile.ngbFile = g_assisiFile.name.left(g_assisiFile.name.lastIndexOf('/')+1) + QString::fromStdString(m_assisiNode["nbg"].as<std::string>());

        auto arenaNode = YAML::LoadFile(g_assisiFile.arenaFile.toStdString());
        QList<QString> layers;
        for(auto it=arenaNode.begin(); it!=arenaNode.end(); it++) layers.append(QString::fromStdString(it->first.as<std::string>()));

        if(layers.size() > 1){
            g_assisiFile.arenaLayer = QInputDialog::getItem(this,tr("Select arena layer"),"",QStringList(layers));
        } else {
            g_assisiFile.arenaLayer = layers[0];
        }

        progress.setMaximum(arenaNode[g_assisiFile.arenaLayer.toStdString()].size());
        progress.show();
        progress.move(ui->arenaSpace->mapToGlobal(QPoint(400-progress.width()/2,400-progress.height()/2)));

        //for(auto it=arenaNode[assisiFile.arenaLayer.toStdString()].begin(); it!=arenaNode[assisiFile.arenaLayer.toStdString()].end(); it++){
        for(auto casu : arenaNode[g_assisiFile.arenaLayer.toStdString()]){
            auto name = QString::fromStdString(casu.first.as<std::string>());
            QPointF coordinates;
            coordinates.setX(400 + 10 * arenaNode[g_assisiFile.arenaLayer.toStdString()][name.toStdString()]["pose"]["x"].as<double>());
            coordinates.setY(400 - 10 * arenaNode[g_assisiFile.arenaLayer.toStdString()][name.toStdString()]["pose"]["y"].as<double>());
            double yaw = arenaNode[g_assisiFile.arenaLayer.toStdString()][name.toStdString()]["pose"]["yaw"].as<double>();

            auto tempZMQ = new QCasuZMQ(this, name);
            auto tempTreeItem = new QCasuTreeItem(tempZMQ);
            auto tempSceneItem = new QCasuSceneItem(coordinates, yaw, tempZMQ);

            connect(tempZMQ, &QCasuZMQ::connectMsg, ui->deployWidget, &QDeploy::append);

            tempTreeItem->setSceneItem(tempSceneItem);
            tempSceneItem->setTreeItem(tempTreeItem);

            ui->casuTree->addTopLevelItem(tempTreeItem);
            m_arenaScene->addItem(tempSceneItem);
            tempTreeItem->setHidden(true);

            tempZMQ->setAddresses(QString::fromStdString(arenaNode[g_assisiFile.arenaLayer.toStdString()][name.toStdString()]["sub_addr"].as<std::string>()),
                               QString::fromStdString(arenaNode[g_assisiFile.arenaLayer.toStdString()][name.toStdString()]["pub_addr"].as<std::string>()),
                               QString::fromStdString(arenaNode[g_assisiFile.arenaLayer.toStdString()][name.toStdString()]["msg_addr"].as<std::string>()));

            progress.setValue(progress.value()+1);
            QApplication::processEvents();
        }
    }
    else if(loadFile.endsWith(".arenaUI")){

        QSettings loadSession(loadFile,QSettings::IniFormat);

        //GENERAL INFORMATION
        g_assisiFile.name = loadSession.value("assisiFile").toString();
        g_assisiFile.arenaLayer = loadSession.value("arenaLayer").toString();

        m_assisiNode = YAML::LoadFile(g_assisiFile.name.toStdString());

        ui->arenaLabel->setText(QString::fromStdString(m_assisiNode["arena"].as<std::string>()));
        ui->neighLabel->setText(QString::fromStdString(m_assisiNode["dep"].as<std::string>()));
        ui->deployLabel->setText(QString::fromStdString(m_assisiNode["nbg"].as<std::string>()));

        ui->deployWidget->setWorkingDirectory(g_assisiFile.name.left(g_assisiFile.name.lastIndexOf('/')));

        g_assisiFile.arenaFile = g_assisiFile.name.left(g_assisiFile.name.lastIndexOf('/')+1) + QString::fromStdString(m_assisiNode["arena"].as<std::string>());
        g_assisiFile.depFile = g_assisiFile.name.left(g_assisiFile.name.lastIndexOf('/')+1) + QString::fromStdString(m_assisiNode["dep"].as<std::string>());
        g_assisiFile.ngbFile = g_assisiFile.name.left(g_assisiFile.name.lastIndexOf('/')+1) + QString::fromStdString(m_assisiNode["nbg"].as<std::string>());

        auto arenaNode = YAML::LoadFile(g_assisiFile.arenaFile.toStdString());
        QMap<QString, QCasuZMQ *> linker;

        //LOAD AND SPAWN CASUs
        loadSession.beginGroup("sceneHierarchy");
        int tempSize = loadSession.beginReadArray("main");
        progress.setMaximum(progress.maximum() + tempSize);
        groupLoad(&arenaNode, &loadSession, tempSize, &linker, &progress);
        loadSession.endArray();
        loadSession.endGroup();
        sortGraphicsScene();

        //LOAD AND SET CONNECTIONS
        loadSession.beginGroup("connectionSettings");
        tempSize = loadSession.beginReadArray("addresses");
        for(int k = 0; k < tempSize; k++){
            loadSession.setArrayIndex(k);
            linker[loadSession.value("casuName").toString()]->setAddresses(loadSession.value("sub_addr").toString(),
                                                                      loadSession.value("pub_addr").toString(),
                                                                      loadSession.value("msg_addr").toString());
        }
        loadSession.endArray();
        loadSession.endGroup();

        //LOAD AND LINK TREND GRAPHS
        loadSession.beginGroup("trendGraphs");
        tempSize = loadSession.beginReadArray("plot");
        for(int k = 0; k < tempSize; k++){
            loadSession.setArrayIndex(k);
            int graphSize = loadSession.beginReadArray("graph");
            QList<QSharedPointer <zmqData::zmqBuffer> > toAdd;

            for(int i = 0; i < graphSize; i++){
                loadSession.setArrayIndex(i);
                auto casuName = loadSession.value("casuName").toString();
                auto key = static_cast<dataType>(loadSession.value("key").toInt());
                toAdd.append(linker[casuName]->getBuffer(key));
            }
            if(toAdd.size()){
                auto tempWidget = new QTrendPlot(ui->casuTree, ui->groupTree);
                m_trendTab->addWidget(tempWidget);
                tempWidget->addGraphList(toAdd);
                tempWidget->setWindowTitle(g_assisiFile.arenaLayer);
            }

            loadSession.endArray();
        }
        loadSession.endArray();
        loadSession.endGroup();
    }

    this->setWindowTitle("ASSISI - " + loadFile.mid(loadFile.lastIndexOf("/")) + ": " + g_assisiFile.arenaLayer);
}

void ArenaUI::on_actionGroup_triggered()
{

    auto itemList= m_arenaScene->selectedItems();
    if(itemList.size()<2) return;
    m_arenaScene->clearSelection();

    auto tempTreeGroup = new QCasuTreeGroup();
    auto tempSceneGroup = new QCasuSceneGroup();

    tempTreeGroup->setSceneItem(tempSceneGroup);
    tempSceneGroup->setTreeItem(tempTreeGroup);

    tempSceneGroup->addToGroup(itemList);

    ui->groupTree->addTopLevelItem(tempTreeGroup);
    m_arenaScene->addItem(tempSceneGroup);
    tempSceneGroup->setSelected(true);

    this->sortGraphicsScene();
}

void ArenaUI::on_actionUngroup_triggered()
{
    auto itemList= m_arenaScene->selectedItems();
    for(auto& item : itemList)
        if(sCast(item)->isGroup()){
            sgCast(item)->removeFromGroup(item->childItems());
            sCast(item)->deleteTreeItem();
            m_arenaScene->destroyItemGroup(sCast(item));
        }
    this->sortGraphicsScene();
}

void ArenaUI::on_actionConnect_triggered()
{
    bool error = false;
    if (m_arenaScene->selectedItems().size() != 1 || //Check if excactly one object is selected
            sCast(m_arenaScene->selectedItems().first())->isGroup()){ // Check if object is single casu (no children)
        error = true;
    }
    if(error){
        QMessageBox msgBox;
        msgBox.setWindowTitle("ERROR");
        msgBox.setText("Please select one CASU.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return;
    }
    auto item = siCast(m_arenaScene->selectedItems().first());
    auto addrDiag = new QDialogConnect(this, item->getZmqObject()->getAddresses(), item->getZmqObject()->getName());
    if(addrDiag->exec()){
        item->getZmqObject()->setAddresses(addrDiag->getAddresses());
    }
}

void ArenaUI::on_actionToggleLog_triggered()
{
    ui->actionToggleLog->setChecked(g_settings->value("log_on").toBool());
    auto question = QString("Are you sure you want to turn ") + (g_settings->value("log_on").toBool() ? QString("OFF") : QString("ON")) + QString(" logging?");
    auto reply = QMessageBox::question(this, "Toggle Log", question , QMessageBox::Yes|QMessageBox::No);
    if(reply == QMessageBox::Yes) g_settings->setValue("log_on", !g_settings->value("log_on").toBool());
    ui->actionToggleLog->setChecked(g_settings->value("log_on").toBool());
}

void ArenaUI::on_actionPlot_selected_in_same_trend_triggered()
{
    auto tempWidget = new QTrendPlot(ui->casuTree, ui->groupTree);
    m_trendTab->addWidget(tempWidget);

    tempWidget->addSelectedGraphs();
    tempWidget->setWindowTitle(g_assisiFile.arenaLayer);
}

void ArenaUI::on_actionPlot_selected_in_different_trends_triggered()
{
    QList<QSharedPointer <zmqData::zmqBuffer> > bufferList;

    for(int k=0; k < ui->casuTree->topLevelItemCount(); k++){
        bufferList.append(tCast(ui->casuTree->topLevelItem(k))->getBuffers());
    }
    for(int k=0; k < ui->groupTree->topLevelItemCount(); k++){
        bufferList.append(tCast(ui->groupTree->topLevelItem(k))->getBuffers());
    }

    for(auto& buffer : bufferList){
        QList<QSharedPointer <zmqData::zmqBuffer> > tempList;
        tempList.append(buffer);

        auto tempWidget = new QTrendPlot(ui->casuTree, ui->groupTree);
        m_trendTab->addWidget(tempWidget);

        tempWidget->addGraphList(tempList);
        tempWidget->setWindowTitle(g_assisiFile.arenaLayer);
    }
}

void ArenaUI::on_actionSettings_triggered()
{
    auto settingsDiag = new QDialogSettings(this);
    settingsDiag->exec();
}

void ArenaUI::customContextMenu(const QPoint &pos)
{
    auto menu = new QMenu(); //no parent because it inherits background image from QGraphicsView
    QAction* tempAction;

    menu->setAttribute(Qt::WA_DeleteOnClose);

    bool error_single = false;
    if (m_arenaScene->selectedItems().size() != 1 ||
            (m_arenaScene->selectedItems().size() == 1 &&
             m_arenaScene->selectedItems()[0]->childItems().size())){
        error_single = true; // Check if object is single casu
    }

    bool error_group  = true;
    for(auto& item : m_arenaScene->selectedItems())
        if(item->childItems().size()) error_group = false;

    bool error_multiple = m_arenaScene->selectedItems().size() < 2;

    // FIXME: Qt 5.6 QMenu::addAction accepts Qt5 style connect (possible lambda expressions)
    tempAction = menu->addAction(g_settings->value("IR_on").toBool() ? "Hide proximity sensors" : "Show proximity sensors");
    connect(tempAction, &QAction::triggered, [&](){
        g_settings->setValue("IR_on",!g_settings->value("IR_on").toBool());
    });
    tempAction = menu->addAction(g_settings->value("temp_on").toBool() ? "Hide temperature sensors" : "Show temperature sensors");
    connect(tempAction, &QAction::triggered, [&](){
        g_settings->setValue("temp_on",!g_settings->value("temp_on").toBool());
    });
    tempAction = menu->addAction(g_settings->value("vibr_on").toBool() ? "Hide vibration marker" : "Show vibration marker");
    connect(tempAction, &QAction::triggered, [&](){
        g_settings->setValue("vibr_on",!g_settings->value("vibr_on").toBool());
    });
    tempAction = menu->addAction(g_settings->value("air_on").toBool() ? "Hide airflow marker" : "Show airflow marker");
    connect(tempAction, &QAction::triggered, [&](){
        g_settings->setValue("air_on",!g_settings->value("air_on").toBool());
    });
    tempAction = menu->addAction(g_settings->value("avgTime_on").toBool() ? "Hide avg. sample time" : "Show avg. sample time");
    connect(tempAction, &QAction::triggered, [&](){
        g_settings->setValue("avgTime_on",!g_settings->value("avgTime_on").toBool());
    });

    menu->addSeparator();

    tempAction = menu->addAction("Group selected", this,SLOT(on_actionGroup_triggered()));
    tempAction->setShortcut(QKeySequence("Ctrl+G"));
    if(error_multiple) tempAction->setEnabled(false);
    tempAction = menu->addAction("Ungroup selected", this,SLOT(on_actionUngroup_triggered()));
    tempAction->setShortcut(QKeySequence("Ctrl+U"));
    if(error_group) tempAction->setEnabled(false);
    menu->addSeparator();
    tempAction = menu->addAction("Set connection", this,SLOT(on_actionConnect_triggered()));
    tempAction->setShortcut(QKeySequence("Ctrl+C"));
    if(error_single) tempAction->setEnabled(false);

    auto sendMenu = new QMenu("Setpoint");
    if(!m_arenaScene->selectedItems().size()) sendMenu->setEnabled(false);
    menu->addMenu(sendMenu);

    sendMenu->addActions(m_setpointActions);

    menu->popup(ui->arenaSpace->mapToGlobal(pos));
}

void ArenaUI::groupSave(QSettings *saveState, const QList<QGraphicsItem *> &group, const QString &groupName)
{
    saveState->beginWriteArray(groupName);
    int k = 0;
    for(auto& item : group){
        saveState->setArrayIndex(k++);
        if(!sCast(item)->isGroup()){
            saveState->setValue("casuName",siCast(item)->getZmqObject()->getName());
        } else {
            groupSave(saveState, item->childItems(), "group");
        }
    }
    saveState->endArray();
}

QList<QGraphicsItem *> ArenaUI::groupLoad(YAML::Node *arenaNode, QSettings *loadState, const int &groupSize, QMap<QString, QCasuZMQ *> *linkMap, QProgressBar *progress)
{
    QList<QGraphicsItem *> returnGroup;
    for(int k=0; k < groupSize; k++){
        loadState->setArrayIndex(k);
        progress->setValue(progress->value()+1);

        if(loadState->childKeys().size()){
            auto name = loadState->value("casuName").toString();
            QPointF coordinates;
            coordinates.setX(400 + 10 * (*arenaNode)[g_assisiFile.arenaLayer.toStdString()][name.toStdString()]["pose"]["x"].as<double>());
            coordinates.setY(400 - 10 * (*arenaNode)[g_assisiFile.arenaLayer.toStdString()][name.toStdString()]["pose"]["y"].as<double>());
            double yaw = (*arenaNode)[g_assisiFile.arenaLayer.toStdString()][name.toStdString()]["pose"]["yaw"].as<double>();

            auto tempZMQ = new QCasuZMQ(this, name);
            auto tempTreeItem = new QCasuTreeItem(tempZMQ);
            auto tempSceneItem = new QCasuSceneItem(coordinates, yaw, tempZMQ);

            connect(tempZMQ, &QCasuZMQ::connectMsg, ui->deployWidget, &QDeploy::append);

            tempTreeItem->setSceneItem(tempSceneItem);
            tempSceneItem->setTreeItem(tempTreeItem);

            ui->casuTree->addTopLevelItem(tempTreeItem);
            m_arenaScene->addItem(tempSceneItem);
            tempTreeItem->setHidden(true);

            linkMap->insert(name, tempZMQ);

            progress->setValue(progress->value()+1);
            returnGroup.append(tempSceneItem);
        }
        else{
            int tempSize = loadState->beginReadArray("group");
            progress->setMaximum(progress->maximum() + tempSize);

            auto tempTreeGroup = new QCasuTreeGroup();
            auto tempSceneGroup = new QCasuSceneGroup();
            tempTreeGroup->setSceneItem(tempSceneGroup);
            tempSceneGroup->setTreeItem(tempTreeGroup);

            auto itemList= std::move(groupLoad(arenaNode, loadState, tempSize, linkMap, progress));
            tempSceneGroup->addToGroup(itemList);

            ui->groupTree->addTopLevelItem(tempTreeGroup);
            m_arenaScene->addItem(tempSceneGroup);

            tempTreeGroup->setSelected(0);
            loadState->endArray();
            returnGroup.append(tempSceneGroup);
        }

        QApplication::processEvents();
    }
   return returnGroup;
}

void ArenaUI::on_actionSave_triggered()
{
    auto saveFile = QFileDialog::getSaveFileName(this,tr("Save As"),g_settings->value("arenaFolder").toString(),tr("*.arenaUI"));

    if(!saveFile.size())return;
    if(!saveFile.endsWith(".arenaUI")) saveFile += ".arenaUI";
    if(QFile(saveFile).exists())QFile(saveFile).remove();

    QSettings saveState(saveFile,QSettings::IniFormat);

    saveState.setValue("assisiFile",g_assisiFile.name);
    saveState.setValue("arenaLayer",g_assisiFile.arenaLayer);

    // - save CASU graphics scene

    auto tempSelection = m_arenaScene->selectedItems(); // save active selection
    QPainterPath pp;
    pp.addRect(m_arenaScene->sceneRect()); // select all -- selectedItems() doesnt return group children which is a case with items()
    m_arenaScene->setSelectionArea(pp);

    saveState.beginGroup("sceneHierarchy");
    groupSave(&saveState, m_arenaScene->selectedItems(),"main");
    saveState.endGroup();

    pp = pp.subtracted(pp);
    pp.addRect(0,0,0,0);
    m_arenaScene->setSelectionArea(pp);
    for(auto& item : tempSelection){
        item->setSelected(true); // after saving items and groups, return selection as was before
    }

    // - save trend position and graphs

    saveState.beginGroup("trendGraphs");
    saveState.beginWriteArray("plot");
    for(int k=0; k < m_trendTab->count() ; k++){
        auto tempPlot = dynamic_cast<QTrendPlot *>(m_trendTab->itemAt(k)->widget());
        saveState.setArrayIndex(k);
        saveState.beginWriteArray("graph");
        for(int i=0; i < tempPlot->graphCount(); i++){
            saveState.setArrayIndex(i);
            saveState.setValue("casuName",tempPlot->link(tempPlot->graph(i))->getCasuName());
            saveState.setValue("key",static_cast<int>(tempPlot->link(tempPlot->graph(i))->getDataType()));
        }
        saveState.endArray();
    }
    saveState.endArray();
    saveState.endGroup();

    //save connection settins

    saveState.beginGroup("connectionSettings");
    saveState.beginWriteArray("addresses");
    int index = 0;
    for(auto& item : m_arenaScene->items()){
        if(!sCast(item)->isGroup()){
            saveState.setArrayIndex(index++);
            saveState.setValue("casuName",siCast(item)->getZmqObject()->getName());
            auto addresses = std::move(siCast(item)->getZmqObject()->getAddresses());
            saveState.setValue("sub_addr",addresses.at(0));
            saveState.setValue("pub_addr",addresses.at(1));
            saveState.setValue("msg_addr",addresses.at(2));
        }
    }


    saveState.endArray();
    saveState.endGroup();
}

void ArenaUI::on_actionCamera_toggled(bool arg1)
{
    if(arg1){
        //videoStream = new QGstreamerView(sideLayout);
    }
    if(!arg1){
        //videoStream->deleteLater();
    }
}
