#ifndef UCSCROLLBARUTILS_H
#define UCSCROLLBARUTILS_H

#include <QObject>
#include <ubuntutoolkitglobal.h>

class QQuickItem;

UT_NAMESPACE_BEGIN

class UBUNTUTOOLKIT_EXPORT UCScrollbarUtils : public QObject
{
    Q_OBJECT
public:
    explicit UCScrollbarUtils(QObject *parent = 0);

    Q_INVOKABLE static bool isComponentComplete(QQuickItem *item);

};

UT_NAMESPACE_END

#endif // UCSCROLLBARUTILS_H
