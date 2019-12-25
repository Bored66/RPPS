// Description: Наблюдение за событиями
#ifndef LogWatcher_H
#define LogWatcher_H

#include "OSUGlobal.h"

#include <QObject>

class /*GENSHARED_EXPORT*/ LogWatcher : public QObject
{
        Q_OBJECT
    public:
        enum DetailLevelType
        {
            Full    = 0, // Error, Warning, Info
            Partial = 1, // Error, Warning
            Minimum = 2  // Error
        };
        enum InfoType
        {
            Error   = 2,
            Warning = 1,
            Info    = 0
        };
        explicit                    LogWatcher(QObject *parent = 0);
        DetailLevelType             DetailLevel() const;
        void                        SetDetailLevel( const DetailLevelType& );
        InfoType                    Type() const;
        void                        SetType( const InfoType& );

        static LogWatcher*          Instance();
        static  void                Log( LogWatcher::InfoType type, const QString& message );
    protected:
        virtual QString             GetOutString( const QString& message );
    Q_SIGNALS:
        void                        signTypeChanged( const InfoType& );
    public Q_SLOTS:
//        void                        OnBroadcastLog( const QString & msg );
        void                        OnError( const QString& errorMessage );
        void                        OnWarning( const QString& warningMessage );
        void                        OnInfo( const QString& infoMessage );
    private:
        DetailLevelType             m_DetailLevel;
        InfoType                    m_InfoType;

        void                        SendLog( const InfoType& code, const QString& message );
};

#endif // LogWatcher_H
