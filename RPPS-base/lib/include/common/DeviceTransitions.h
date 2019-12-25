// Description: Специализированные классы переходов между состояниями
#ifndef DEVICETRANSITIONS_H
#define DEVICETRANSITIONS_H

#include <QStateMachine>
#include "CommandState.h"

class SignalTransitionWithTest : public QSignalTransition
{
public:
    SignalTransitionWithTest
    (
        QObject *sender,
        const char* signal,
        QState *sourceState = 0
    ):
        QSignalTransition(sender, signal, sourceState)
    {}
protected:
    virtual bool testArgs(const QList<QVariant> & arguments) const = 0;
    bool eventTest(QEvent *e)
    {
        if (!QSignalTransition::eventTest(e))
            return false;
        QStateMachine::SignalEvent *se =
                static_cast<QStateMachine::SignalEvent*>(e);
        return testArgs(se->arguments());
    }
};

template<typename CUSTOM, int INDEX = 0, typename IDType=uint>
class CheckIdTransition : public SignalTransitionWithTest
{
public:
    CheckIdTransition
    (
        QObject *observer,
        const char* signal,
        IDType id,
        QState *sourceState = 0
    ):
        SignalTransitionWithTest(observer, signal, sourceState),
       id(id)
    {}
protected:
    bool testArgs(const QList<QVariant> & arguments) const override
    {
        QVariant var = arguments.at(INDEX);
        if (var.canConvert(QVariant::Type(qMetaTypeId<CUSTOM>())) )
        {
            CUSTOM msg = var.value<CUSTOM>();
            return test(msg);
        }
        return false;
    }
    virtual
    bool test(const CUSTOM & msg) const
    {
        return msg.GetId() == id;
    }
private:
    IDType id;
};

template<typename RESRET, typename...Types>
class ConditionalTransition : public SignalTransitionWithTest
{
public:
    ConditionalTransition
    (
        QObject *observer,
        const char* signal,
        Types... values,
        QState *sourceState = 0
    ):
        SignalTransitionWithTest(observer, signal, sourceState),
        condValues(values...)
    {
    }
    QList<QVariant> condValues;
};
template< typename CommState, typename Command >
class DeviceClientTask : public CommState
{
public:
    DeviceClientTask
    (
        Command command,
        const QString & name,
        QState * parent = nullptr
    ):
        CommState(command, name, parent)
    {}
    using CommandState::onReceiveStartNext;
    template<typename Trans>
    CommandState& onReceiveStartNext( Trans *transition, QState* next)
    {
        this->addTransition(transition);
        transition->setTargetState(next);
        return *this;
    }
    template<typename CUSTOM, typename IDType=uint>
    DeviceClientTask& onCorrectIdStartNext
    (
        const char* signal,
        QObject *observer,
        IDType id,
        QState* next)
    {
        CheckIdTransition<CUSTOM> *trans =
                new CheckIdTransition<CUSTOM>(observer, signal,  id, this);
        return dynamic_cast<DeviceClientTask&> (onReceiveStartNext(trans, next));
    }
    DeviceClientTask& onTimeout(QState *nextTask, int timeout)
    {
        this->addNextWithDelay(timeout, nextTask);
        return *this;
    }
};
class DeviceAdapterTask :
        public DeviceClientTask<AdapterCommandState,
                AdapterCommandState::AdapterCommand>
{
public:
    DeviceAdapterTask
    (
        AdapterCommand command,
        const QString & name,
        QState * parent = nullptr
    ):
        DeviceClientTask(command, name, parent)
    {}
};

#endif // DEVICETRANSITIONS_H
