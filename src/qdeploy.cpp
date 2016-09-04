#include "qdeploy.h"

QDeploy::QDeploy(QWidget *parent) :
    QTextEdit(parent),
    m_simulatorPID(0)
{
    setReadOnly(true);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
   // setTextFormat(Qt::PlainText);
   // setWordWrap(true);

    m_shell = new QProcess;
}

QDeploy::~QDeploy()
{
    m_shell->close();
    delete m_shell;
}


void QDeploy::setWorkingDirectory(const QString &dir)
{
    m_shell->setWorkingDirectory(dir);
}

bool QDeploy::isSimulatorStarted()
{
    auto tempString = g_settings->value("simulator").toString();
    tempString = tempString.right(tempString.size() - tempString.lastIndexOf("/") - 1);
    tempString = tempString.left(15);

    m_shell->start("sh");
    m_shell->write("pgrep ");
    m_shell->write(tempString.toStdString().c_str());
    m_shell->closeWriteChannel();
    m_shell->waitForFinished();

    QString out(m_shell->readAll());
    if(out.size()) m_simulatorPID = out.toLongLong();
    return out.size();
}

void QDeploy::deploy()
{
    if(m_shell->state() == QProcess::NotRunning){
        m_shell->start("deploy.py", QStringList(g_assisiFile.name));
        append("[arenaUI] Starting 'deploy.py'");
    }
    else append("[arenaUI] Already running a process");
}

void QDeploy::run()
{
    if(m_shell->state() == QProcess::NotRunning){
        m_shell->start("assisirun.py", QStringList(g_assisiFile.name));
        append("[arenaUI] Starting 'assisirun.py'");
    }
    else append("[arenaUI] Already running a process");
}

void QDeploy::stop()
{
    if(m_shell->state() != QProcess::NotRunning) m_shell->close();
    else append("[arenaUI] Process is not running");
}

void QDeploy::collect()
{
    if(m_shell->state() == QProcess::NotRunning){
        m_shell->start("collect_data.py", QStringList(g_assisiFile.name));
        append("[arenaUI] Starting 'collect_data.py'");
    }
    else append("[arenaUI] Already running a process");
}

void QDeploy::cleanLog()
{

}

void QDeploy::simulatorStart()
{
    if(m_shell->state() == QProcess::NotRunning){
        if(isSimulatorStarted()){
            append("[Simulator] Simulator is already running (PID: " + QString::number(m_simulatorPID) +")");
            return;
        }
        m_shell->startDetached(g_settings->value("simulator").toString(), QStringList(), QString(), &m_simulatorPID);
        if(!m_simulatorPID) append("[Simulator] Cannot start: " + g_settings->value("simulator").toString());
        else append("[Simulator] New simulator started (PID: " + QString::number(m_simulatorPID) +")");
    }
    else append("[arenaUI] Already running a process");

    m_shell->start("sim.py", QStringList(g_assisiFile.arenaFile));
}

void QDeploy::simulatorStop()
{
    if(isSimulatorStarted()){
        m_shell->execute(QString("kill ") + QString::number(m_simulatorPID));
        append("[Simulator] Simulator process is stopped.");
    }
}

void QDeploy::toggleOutput(bool state){
    if(state){

        m_shellOut1 = connect(m_shell, &QProcess::readyReadStandardOutput, [&](){
            append("\n[SHELL][OUT]");
            append(m_shell->readAllStandardOutput());
        });
        m_shellOut2 = connect(m_shell, &QProcess::readyReadStandardError, [&](){
            append("\n[SHELL][ERR]");
            append(m_shell->readAllStandardError());
        });
    }
    else{
        disconnect(m_shellOut1);
        disconnect(m_shellOut2);
    }

}
