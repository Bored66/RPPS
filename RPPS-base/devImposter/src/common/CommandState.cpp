// Description: Специализированные классы сотояний
#include "CommandState.h"

TransportRequestStatus CommandState::sendCommand()
{
    if (transport->hasError())
        qDebug().noquote() << __FUNCTION__  << transport->ErrorString();
    return transport->SendRequest(command);

}
void CommandState::addNextWithDelay(int msec, QState *state)
{
    machine()->addState(state);
    addTimeoutTransition(msec, state);
}
void CommandState::addTimeoutTransition(int msec, QState *state)
{
    watchDog.setSingleShot(true);
    watchDog.setInterval(msec);
    addTransition(&watchDog, SIGNAL(timeout()), state);
}
void CommandState::startingWatchDogTimer()
{
    if (watchDog.isSingleShot())
        watchDog.start();
    timeElapsed.start();
}
void CommandState::entering()
{
    qDebug().noquote() << "Entered Sending " + command;
    startingWatchDogTimer();
}
void CommandState::onEntry(QEvent *event)
{
    QStateMachine::SignalEvent *signal =
            dynamic_cast<QStateMachine::SignalEvent*>(event);
    if (signal)
        signal->arguments();

    entering();
    if ( sendCommand() != REQUEST_SEND_SUCCESS)
        emit transportError();
}
void CommandState::onExit(QEvent *event)
{
    Q_UNUSED(event)
    qDebug().noquote() << QString("Exiting Sending %1 elapsed %2 ms")
                .arg((QString)(IsDataPrintable( command )?
                                   command : command.toHex()))
                .arg(timeElapsed.elapsed() );
}
TransportRequestStatus AdapterCommandState::sendCommand()
{
    return adapterCommand();
}
TransportRequestStatus CloseTransportState::sendCommand()
{
    if (transport->hasError())
        qDebug().noquote() << __FUNCTION__ << transport->ErrorString();
    transport->CloseConnection();
    return REQUEST_SEND_SUCCESS;
}
