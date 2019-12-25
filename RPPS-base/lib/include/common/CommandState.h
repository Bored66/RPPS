// Description: Специализированные классы сотояний
#ifndef COMMANDSTATE_H
#define COMMANDSTATE_H

#include <functional>
#include <QObject>
#include <QTime>
#include <QTimer>
#include <QStateMachine>
#include "Transport.h"

class CommandState : public QState
{
    Q_OBJECT
public:
    explicit
    CommandState(const QByteArray & command, ITransport *transport, QState * parent = nullptr):
        QState(parent),
        transport{transport},
        command{command}
    {}
  virtual TransportRequestStatus sendCommand();

signals:
    void transportError();
public slots:
    void addNextWithDelay(int msec, QState *state);
    void addTimeoutTransition(int msec, QState *state);
    CommandState& onReceiveStartNext( const char* signal, QObject *sender, QState* next)
    {
        this->addTransition(sender, signal, next);
        return *this;
    }
    void addTransitionTo(QSignalTransition *transition, QState* next)
    {
        this->addTransition(transition->senderObject(), transition->signal(), next);
    }
    void startingWatchDogTimer();
    void entering();
protected:
    void onEntry(QEvent *event) override;
    void onExit(QEvent *event) override;
    ITransport *transport;
    QByteArray command;
private:
    QTime timeElapsed;
    QTimer watchDog;
};
class AsciiCommandState : public CommandState
{
public:
    explicit
    AsciiCommandState(const QString & command, ITransport *transport, QState * parent = nullptr):
        CommandState(command.simplified().toLocal8Bit(), transport, parent)
    {}
};
class CloseTransportState : public AsciiCommandState
{
public:
    explicit CloseTransportState(ITransport *transport, QState * parent = nullptr):
        AsciiCommandState("CloseTransportState", transport, parent)
    {}
    TransportRequestStatus sendCommand() override;
};
class AdapterCommandState : public AsciiCommandState
{
public:
    using AdapterCommand = std::function<TransportRequestStatus ()>;
    AdapterCommandState(AdapterCommand command, const QString & name, QState * parent = nullptr):
        AsciiCommandState(name, nullptr, parent),
        adapterCommand{command}
    {}
    TransportRequestStatus sendCommand() override;
private:
    AdapterCommand adapterCommand;
};

#endif // COMMANDSTATE_H
