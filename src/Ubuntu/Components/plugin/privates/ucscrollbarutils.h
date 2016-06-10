#ifndef UCSCROLLBARUTILS_H
#define UCSCROLLBARUTILS_H

#include <QObject>

class QQuickFlickable;
class UCScrollbarUtils : public QObject
{
    Q_OBJECT
public:
    explicit UCScrollbarUtils(QObject *parent = 0);

    Q_INVOKABLE bool isFlickableComplete(QQuickFlickable *flickable);

};

#endif // UCSCROLLBARUTILS_H
