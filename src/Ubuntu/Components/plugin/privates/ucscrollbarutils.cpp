#include "ucscrollbarutils.h"
#include <QtQuick/private/qquickflickable_p.h>
#include <QtQuick/private/qquickitem_p.h>

UCScrollbarUtils::UCScrollbarUtils(QObject *parent) : QObject(parent)
{
}

bool UCScrollbarUtils::isFlickableComplete(QQuickFlickable *flickable)
{
    if (flickable == Q_NULLPTR) {
        qDebug() << "ScrollbarUtils: null Flickable parameter.";
        return false;
    } else {
        QQuickItemPrivate *f = QQuickItemPrivate::get(flickable);
        if (f == Q_NULLPTR) {
            //don't fall apart, but give a warning
            qDebug() << "ScrollbarUtils: failed to get flickable's pimpl.";
            return false;
        }
        return f->componentComplete;
    }
}
