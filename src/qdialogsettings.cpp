#include "qdialogsettings.h"
#include "ui_qdialogsettings.h"

QDialogSettings::QDialogSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QDialogSettings)
{
    ui->setupUi(this);

    ui->trendTimeSpan->setTime(g_settings->value("trendTimeSpan").toTime());
    ui->trendSampleTime->setValue(g_settings->value("trendSampleTime_ms").toInt());
    ui->logFolder->setText(g_settings->value("logFolder").toString());
    ui->camFolder->setText(g_settings->value("camFolder").toString());
    ui->arenaFolder->setText(g_settings->value("arenaFolder").toString());
    ui->simFile->setText(g_settings->value("simulator").toString());
    ui->forceLog->setChecked(g_settings->value("forceLog").toBool());
    ui->forceScene->setChecked(g_settings->value("forceScene").toBool());

    link[ui->searchArena] = ui->arenaFolder;
    link[ui->searchLog] = ui->logFolder;
    link[ui->searchCam] = ui->camFolder;
    link[ui->searchSim] = ui->simFile;

}

QDialogSettings::~QDialogSettings()
{
    delete ui;
}

void QDialogSettings::findFolder()
{
    QString temp;
    if(link[qobject_cast<QPushButton *>(QObject::sender())] == ui->simFile)
        temp = QFileDialog::getOpenFileName(this,tr("Select simulator"), link[qobject_cast<QPushButton *>(QObject::sender())]->text());
    else temp = QFileDialog::getExistingDirectory(this,tr("Select folder"), link[qobject_cast<QPushButton *>(QObject::sender())]->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(temp.size()) link[qobject_cast<QPushButton *>(QObject::sender())]->setText(temp);
}

void QDialogSettings::saveConfig()
{
    g_settings->setValue("logFolder", ui->logFolder->text() + (ui->logFolder->text().endsWith("/")? "" : "/"));
    g_settings->setValue("camFolder", ui->camFolder->text() + (ui->camFolder->text().endsWith("/")? "" : "/"));
    g_settings->setValue("arenaFolder", ui->arenaFolder->text() + (ui->arenaFolder->text().endsWith("/")? "" : "/"));
    g_settings->setValue("simulator", ui->simFile->text());
    g_settings->setValue("trendTimeSpan", QTime::fromString(ui->trendTimeSpan->text(),"mm:ss"));
    g_settings->setValue("trendSampleTime_ms", ui->trendSampleTime->text());
    g_settings->setValue("forceLog", ui->forceLog->isChecked());
    g_settings->setValue("forceScene", ui->forceScene->isChecked());


    g_settings->setValue("logSubFolder", g_settings->value("logFolder").toString() + QDate::currentDate().toString("yy-MM-dd") + "/");
    g_settings->setValue("camSubFolder", g_settings->value("camFolder").toString() + QDate::currentDate().toString("yy-MM-dd") + "/");

    if(!QDir(g_settings->value("logSubFolder").toString()).exists())QDir().mkdir(g_settings->value("logSubFolder").toString());
    if(!QDir(g_settings->value("camSubFolder").toString()).exists())QDir().mkdir(g_settings->value("camSubFolder").toString());

    emit accept();
}
