#include "qdeploy.h"

QDeploy::QDeploy(QWidget *parent) :
    QTextEdit(parent),
    _simulatorPID(0)
{
    setReadOnly(true);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
   // setTextFormat(Qt::PlainText);
   // setWordWrap(true);

    _shell = new QProcess;
}

QDeploy::~QDeploy()
{
    _shell->close();
    delete _shell;
}


void QDeploy::setWorkingDirectory(QString dir)
{
    _shell->setWorkingDirectory(dir);
}

bool QDeploy::isSimulatorStarted()
{
    auto tempString = settings->value("simulator").toString();
    tempString = tempString.right(tempString.size() - tempString.lastIndexOf("/") - 1);
    tempString = tempString.left(15);

    _shell->start("sh");
    _shell->write("pgrep ");
    _shell->write(tempString.toStdString().c_str());
    _shell->closeWriteChannel();
    _shell->waitForFinished();

    QString out(_shell->readAll());
    if(out.size()) _simulatorPID = out.toLongLong();
    return out.size();
}

void QDeploy::deploy()
{
    if(_shell->state() == QProcess::NotRunning){
        _shell->start("deploy.py", QStringList(assisiFile.name));
        append("[arenaUI] Starting 'deploy.py'\n");
    }
    else append("[arenaUI] Already running a process\n");
}

void QDeploy::run()
{
    if(_shell->state() == QProcess::NotRunning){
        _shell->start("assisirun.py", QStringList(assisiFile.name));
        append("[arenaUI] Starting 'assisirun.py'\n");
    }
    else append("[arenaUI] Already running a process\n");
}

void QDeploy::stop()
{
    if(_shell->state() != QProcess::NotRunning) _shell->close();
    else append("[arenaUI] Process is not running\n");
}

void QDeploy::collect()
{
    if(_shell->state() == QProcess::NotRunning){
        _shell->start("collect_data.py", QStringList(assisiFile.name));
        append("[arenaUI] Starting 'collect_data.py'\n");
    }
    else append("[arenaUI] Already running a process\n");
}

void QDeploy::cleanLog()
{

}

void QDeploy::simulatorStart()
{
    if(_shell->state() == QProcess::NotRunning){
        if(isSimulatorStarted()){
            append("[Simulator] Simulator is already running (PID: " + QString::number(_simulatorPID) +")\n");
            return;
        }
        _shell->startDetached(settings->value("simulator").toString(), QStringList(), QString(), &_simulatorPID);
        if(!_simulatorPID) append("[Simulator] Cannot start: " + settings->value("simulator").toString() + "\n");
        else append("[Simulator] New simulator started (PID: " + QString::number(_simulatorPID) +")\n");
    }
    else append("[arenaUI] Already running a process\n");

    _shell->start("sim.py", QStringList(assisiFile.arenaFile));
}

void QDeploy::simulatorStop()
{
    if(isSimulatorStarted()){
        _shell->execute(QString("kill ") + QString::number(_simulatorPID));
        append("[Simulator] Simulator process is stopped.\n");
    }
}

void QDeploy::toggleOutput(bool state){
    if(state){

        _shellOut1 = connect(_shell, &QProcess::readyReadStandardOutput, [&](){
            append("[SHELL][OUT]");
            append(_shell->readAllStandardOutput());
        });
        _shellOut2 = connect(_shell, &QProcess::readyReadStandardError, [&](){
            append("[SHELL][ERR]");
            append(_shell->readAllStandardError());
        });
    }
    else{
        disconnect(_shellOut1);
        disconnect(_shellOut2);
    }

}
