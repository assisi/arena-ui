#include "qdeploy.h"

QDeploy::QDeploy(QWidget *parent) :
    QLabel(parent),
    _simulatorPID(0)
{
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setTextFormat(Qt::PlainText);
    setWordWrap(true);

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

void QDeploy::appendText(QString text)
{
    setText(this->text() + text);
}

bool QDeploy::isSimulatorStarted()
{
    _shell->start("sh");
    _shell->write("ps -e -o command | grep \"");
    _shell->write(settings->value("simulator").toString().toStdString().c_str());
    _shell->write("\"");
    _shell->closeWriteChannel();
    _shell->waitForFinished();
    QString out(_shell->readAll());
    return out.indexOf("grep");
}

void QDeploy::deploy()
{
    if(_shell->state() == QProcess::NotRunning){
        _shell->start("deploy.py", QStringList(assisiFile.name));
        appendText("[arenaUI] Starting 'deploy.py'\n");
    }
    else appendText("[arenaUI] Already running a process\n");
}

void QDeploy::run()
{
    if(_shell->state() == QProcess::NotRunning){
        _shell->start("assisirun.py", QStringList(assisiFile.name));
        appendText("[arenaUI] Starting 'assisirun.py'\n");
    }
    else appendText("[arenaUI] Already running a process\n");
}

void QDeploy::stop()
{
    if(_shell->state() != QProcess::NotRunning) _shell->close();
    else appendText("[arenaUI] Process is not running\n");
}

void QDeploy::collect()
{
    if(_shell->state() == QProcess::NotRunning){
        _shell->start("collect_data.py", QStringList(assisiFile.name));
        appendText("[arenaUI] Starting 'collect_data.py'\n");
    }
    else appendText("[arenaUI] Already running a process\n");
}

void QDeploy::cleanLog()
{

}

void QDeploy::simulatorStart()
{
    if(_shell->state() == QProcess::NotRunning){
        if(isSimulatorStarted()){
            if(_simulatorPID) appendText("[Simulator] Simulator is running (PID: " + QString::number(_simulatorPID) +")\n");
            else appendText("[Simulator] Simulator is running outside this program.");
            return;
        }
        _shell->startDetached(settings->value("simulator").toString(), QStringList(), QString(), &_simulatorPID);
        if(_simulatorPID == 0) appendText("[Simulator] Cannot start: " + settings->value("simulator").toString() + "\n");
        else appendText("[Simulator] New simulator started (PID: " + QString::number(_simulatorPID) +")\n");
    }
    else appendText("[arenaUI] Already running a process\n");

    _shell->start("sim.py", QStringList(assisiFile.arenaFile));
}

void QDeploy::simulatorStop()
{
    if(isSimulatorStarted()){
        if(_simulatorPID){
            _shell->execute(QString("kill ") + QString::number(_simulatorPID));
            appendText("[Simulator] Simulator process is stopped.");
        }
        else appendText("[Simulator] Simulator is running outside this program.");
    }
    _simulatorPID = 0;
}

void QDeploy::appendOut()
{
    appendText("[SHELL][OUT] ");
    appendText(_shell->readAllStandardOutput());
}

void QDeploy::appendErr()
{
    appendText("[SHELL][ERR] ");
    appendText(_shell->readAllStandardError());
}

void QDeploy::toggleOutput(int state){
    if(state){
        _shell->readAllStandardError();
        _shell->readAllStandardOutput();
        connect(_shell, SIGNAL(readyReadStandardOutput()), this, SLOT(appendOut()));
        connect(_shell, SIGNAL(readyReadStandardError()), this, SLOT(appendErr()));
    }
    else{
        disconnect(_shell, SIGNAL(readyReadStandardOutput()), this, SLOT(appendOut()));
        disconnect(_shell, SIGNAL(readyReadStandardError()), this, SLOT(appendErr()));
    }

}
