// Description: Классы конечных автоматов (машин состояний) для устройств
#ifndef DEVICESTATEMACHINE_H
#define DEVICESTATEMACHINE_H

#include <QObject>

#include "CommandState.h"

class QStateMachine;
class DeviceAdapterTask;
class DeviceStateMachine : public QObject
{
    Q_OBJECT
public:
    DeviceStateMachine(QObject *parent = nullptr):
        QObject(parent)
    {
        machine = new QStateMachine();
    }
    virtual void setup() = 0;
    void setOperationTimeout(int millisec)
    {
        operationTimeout = millisec;
    }
    virtual void start()
    {
        if (machine)
        {
            setup();
            machine->start();
        }
    }
    void restart()
    {
        stopRunningStateMachine();
        machine->start();
    }
    void stopRunningStateMachine()
    {
        if (machine->isRunning())
        {
            qDebug().noquote() << "Machine is running - stopping";
            machine->stop();
        }
        else
        {
            qDebug().noquote() << "Machine is not running - " +
                        (machine->error()?
                             machine->errorString() +
                             QString::number( machine->error() ):
                             "restarting");
        }
    }
    void recreate()
    {
        machine->deleteLater();
        machine = nullptr;
        machine = new QStateMachine();
    }
    template< typename CommandSt, typename CommandCont >
    CommandState* makeCommandChain
    (
        CommandSt *nextCommand,
        const CommandCont& cmdSeq,
        ITransport *transport
    )
    {
        CommandSt *firstCmd{nullptr}, *prev{nullptr};
        for (auto command : cmdSeq)
        {
            CommandSt *nextCmd = new CommandSt(command, transport);
            if (firstCmd == nullptr)
            {
                firstCmd = nextCmd;
                machine->addState(firstCmd);
            }
            if (prev)
                prev->addNextWithDelay(operationTimeout, nextCmd);
            prev = nextCmd;
        }
        prev->addTimeoutTransition(operationTimeout, nextCommand);
        return firstCmd;
    }
protected:
    QStateMachine *machine{nullptr};
    int operationTimeout = 1700;
};
class IAsyncDeviceAction
{
public:
    void    action();
};
class CommandSeqWithSignalTransition : public DeviceStateMachine
{
    Q_OBJECT
signals:
    void timedOut();
    void done();
public:
    CommandSeqWithSignalTransition()
    {}
    template< typename CommandSt, typename CommandCont, typename Msg >
    CommandState* setCommandChain
    (
        const CommandCont& cmdSeq,
        ITransport *transport,
        QSignalTransition *transition,
        std::function<void(Msg&)> beforeSend,
        CommandSt *nextCommand = nullptr
    )
    {
        CommandSt *firstCmd{nullptr}, *prev{nullptr};
        for (auto command : cmdSeq)
        {
            CommandSt *cmd = new CommandSt(command.PrepareToSend(), transport);
            cmd->addTimeoutTransition(operationTimeout, &timeoutError);
            machine->addState(cmd);
            connect(cmd, &QState::entered, [&command, beforeSend](){
              beforeSend(command);
            });
            if (firstCmd == nullptr)
            {
                firstCmd = cmd;
                machine->setInitialState(firstCmd);
            }
            if (prev)
                prev->addTransitionTo(transition, cmd);
            prev = cmd;
        }
        /*if (nextCommand)
            prev->addTransitionTo(transition, nextCommand);*/
        prev->addTransition(transition->senderObject(), transition->signal(), &final);
        return firstCmd;
    }
    void setup() override
    {
        if (machine)
        {
            machine->addState(&final);
            connect(machine, SIGNAL(finished()), SIGNAL(done()));
            machine->addState(&timeoutError);
            connect(&timeoutError, SIGNAL(propertiesAssigned()), SIGNAL(timedOut()));
        }
    }
private:
    QFinalState final;
    QState timeoutError;
};
#endif // DEVICESTATEMACHINE_H
