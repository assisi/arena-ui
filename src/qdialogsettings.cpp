#include "qdialogsettings.h"
#include "ui_qdialogsettings.h"

QDialogSettings::QDialogSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QDialogSettings)
{
    ui->setupUi(this);

    ui->trendTimeSpan->setTime(settings->value("trendTimeSpan").toTime());
    ui->trendSampleTime->setValue(settings->value("trendSampleTime_ms").toInt());
    ui->logFolder->setText(settings->value("logFolder").toString());
    ui->camFolder->setText(settings->value("camFolder").toString());
    ui->arenaFolder->setText(settings->value("arenaFolder").toString());
    ui->simFile->setText(settings->value("simulator").toString());
    ui->forceLog->setChecked(settings->value("forceLog").toBool());
    ui->forceScene->setChecked(settings->value("forceScene").toBool());

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
    settings->setValue("logFolder", ui->logFolder->text() + (ui->logFolder->text().endsWith("/")? "" : "/"));
    settings->setValue("camFolder", ui->camFolder->text() + (ui->camFolder->text().endsWith("/")? "" : "/"));
    settings->setValue("arenaFolder", ui->arenaFolder->text() + (ui->arenaFolder->text().endsWith("/")? "" : "/"));
    settings->setValue("simulator", ui->simFile->text());
    settings->setValue("trendTimeSpan", QTime::fromString(ui->trendTimeSpan->text(),"mm:ss"));
    settings->setValue("trendSampleTime_ms", ui->trendSampleTime->text());
    settings->setValue("forceLog", ui->forceLog->isChecked());
    settings->setValue("forceScene", ui->forceScene->isChecked());


    settings->setValue("logSubFolder", settings->value("logFolder").toString() + QDate::currentDate().toString("yy-MM-dd") + "/");
    settings->setValue("camSubFolder", settings->value("camFolder").toString() + QDate::currentDate().toString("yy-MM-dd") + "/");

    if(!QDir(settings->value("logSubFolder").toString()).exists())QDir().mkdir(settings->value("logSubFolder").toString());
    if(!QDir(settings->value("camSubFolder").toString()).exists())QDir().mkdir(settings->value("camSubFolder").toString());

    emit accept();
}
