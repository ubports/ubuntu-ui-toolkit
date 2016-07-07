#ifndef UCSCROLLBARUTILS_H
#define UCSCROLLBARUTILS_H

#include <QObject>
#include <ubuntutoolkitglobal.h>

class QQuickFlickable;

UT_NAMESPACE_BEGIN

class UBUNTUTOOLKIT_EXPORT UCScrollbarUtils : public QObject
{
    Q_OBJECT
public:
    explicit UCScrollbarUtils(QObject *parent = 0);

    Q_INVOKABLE static bool isFlickableComplete(QQuickFlickable *flickable);

};

UT_NAMESPACE_END

#endif // UCSCROLLBARUTILS_H
