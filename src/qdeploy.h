#ifndef QDEPLOY_H
#define QDEPLOY_H

#include <QDebug>
#include <QTextEdit>
#include <QProcess>
#include <QWindow>

#include <QThread>

#include "globalHeader.h"

/*!
 * \brief Subclassed QLabel object for deployment and simulator tasks
 *
 * Deployment is done with shell commands and output can be collected and shown in UI
 *
 * Simulator is started as individual process with spawning capabilities
 */
class QDeploy : public QTextEdit
{
    Q_OBJECT

public:
    QDeploy(QWidget *parent = 0);
    ~QDeploy();

    void setWorkingDirectory(QString dir);
private:
    /*!
     * \brief Individual process for executing shell commands
     */
    QProcess* _shell;

    qint64 _simulatorPID;

    QMetaObject::Connection _shellOut1;
    QMetaObject::Connection _shellOut2;

    bool isSimulatorStarted();

signals:

public slots:
    /*!
     * \brief Deploy program specified in project file (*.assisi)
     */
    void deploy();
    /*!
     * \brief Run program (it needs to be deployed first)
     */
    void run();
    /*!
     * \brief Stop current shell action
     */
    void stop();
    /*!
     * \brief Collect data log from CASUs after running a deployed program
     */
    void collect();
    /*!
     * \brief Clean data log on CASUs (EMPTY - collect() automaticly removes collected logs)
     */
    void cleanLog();
    /*!
     * \brief Toggle QProcess output collecting
     * \param state
     */
    void toggleOutput(bool state);

    /*!
     * \brief Start simulator and spawn CASUs from loaded arena file (*.arena)
     */
    void simulatorStart();
    /*!
     * \brief Close running simulator
     */
    void simulatorStop();
};

#endif // QDEPLOY_H
