// Description: Наблюдение за событиями
#include "LogWatcher.h"
#include "Broadcaster.h"
#include "OSULiterals.h"

#include <QDateTime>
#include <QPointer>

static QSharedPointer<LogWatcher> m_Instance;

//------------------------------------------------------------------------------
LogWatcher::LogWatcher(QObject *parent)
    : QObject(parent)
    , m_DetailLevel ( DetailLevelType::Full )
    , m_InfoType    ( InfoType::Info )
{
}

//------------------------------------------------------------------------------
LogWatcher::DetailLevelType
LogWatcher::DetailLevel() const
{
    return m_DetailLevel;
}

//------------------------------------------------------------------------------
void
LogWatcher::SetDetailLevel(const LogWatcher::DetailLevelType& levelType)
{
    m_DetailLevel = levelType;
}

//------------------------------------------------------------------------------
LogWatcher::InfoType
LogWatcher::Type() const
{
    return m_InfoType;
}

//------------------------------------------------------------------------------
void
LogWatcher::SetType(const LogWatcher::InfoType& infoType)
{
    m_InfoType = infoType;
    emit signTypeChanged( Type() );
}

//------------------------------------------------------------------------------
LogWatcher*
LogWatcher::Instance()
{
    if( m_Instance.isNull() )
        m_Instance = QSharedPointer<LogWatcher>(new LogWatcher());

    return m_Instance.data();
}

//------------------------------------------------------------------------------
void
LogWatcher::Log(LogWatcher::InfoType type, const QString& message)
{
    QPointer<LogWatcher> instance = LogWatcher::Instance();
    if( !instance.isNull() )
        switch ( type )
        {
            case LogWatcher::InfoType::Error:
            {
                instance->OnError( message );
                break;
            }
            case LogWatcher::InfoType::Info:
            {
                instance->OnInfo( message );
                break;
            }
            case LogWatcher::InfoType::Warning:
            {
                instance->OnWarning( message );
                break;
            }
            default: ;
        }
}

//------------------------------------------------------------------------------
QString
LogWatcher::GetOutString(const QString& message)
{
    QString retval = message;
    QString prefix = QString("%1. %2").arg( QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss")).arg("%1: %2");

    if( !retval.isEmpty() )
        switch( Type() )
        {
            case InfoType::Error :
                retval = prefix.arg("ERR").arg( retval );
                break;
            case InfoType::Warning :
                retval = prefix.arg("WRN").arg( retval );
                break;
            case InfoType::Info :
                retval = prefix.arg("INF").arg( retval );
                break;
            default:;
        }

    return retval;
}

//------------------------------------------------------------------------------
void LogWatcher::OnError(const QString& errorMessage)
{
    SendLog(InfoType::Error, errorMessage);
}

//------------------------------------------------------------------------------
void LogWatcher::OnWarning(const QString& warningMessage)
{
    if( DetailLevel() < DetailLevelType::Minimum )
        SendLog(InfoType::Warning, warningMessage);
}

//------------------------------------------------------------------------------
void LogWatcher::OnInfo(const QString& infoMessage)
{
    if( DetailLevel() < DetailLevelType::Partial )
        SendLog(InfoType::Info, infoMessage);
}

//------------------------------------------------------------------------------
void LogWatcher::SendLog(const LogWatcher::InfoType& code, const QString& message)
{
    SetType( code );
    Broadcast::Log( QString("%1 ( Code %2 )").
                    arg(GetOutString( UTF8(message.toStdString().c_str())) ).
                    arg((int)code) );
}
