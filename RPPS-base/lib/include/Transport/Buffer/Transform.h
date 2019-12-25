// Description: Преобразование байт
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <QObject>
#include <QPair>

struct Replacement
{
    QByteArray bytesFrom, bytesTo;
    int offsetLeft, offsetRight;
};

class Transform :
    public                                  QObject
{
    Q_OBJECT
public:
    explicit Transform( QObject * parent = nullptr );

    void                                    SetReplacements(const QList<Replacement> &replacements );
    QByteArray                              Stuff( const QByteArray & data );
    QByteArray                              Unstuff( const QByteArray & data, int & untrusted );
    QPair<QByteArray, QByteArray>           ReplacementLookup(QByteArray subData, int & minIndex);
    void                                    AddReplacementToResult(QByteArray &result, int minIndex, QPair< QByteArray, QByteArray > replacement, QByteArray & subData);
    void                                    DebugOut(const QString &type, QPair< QByteArray, QByteArray > replacement, const QByteArray data, QByteArray result, int minIndex);
    void setDebugResult(bool value);
    QList< Replacement >                    Replacements() const;
private:
    int                                     UntrustedBytesCount( const QByteArray & data );
private:
    QList< Replacement >                    mReplacements;
    bool debugResult = false;
};

#endif //TRANSFORM_H
