#include "ucscrollbarutils.h"
#include <QtQuick/private/qquickitem_p.h>

UT_NAMESPACE_BEGIN

UCScrollbarUtils::UCScrollbarUtils(QObject *parent) : QObject(parent)
{
}

bool UCScrollbarUtils::isComponentComplete(QQuickItem *item)
{
    if (item == Q_NULLPTR) {
        qDebug() << "ScrollbarUtils: null Item parameter.";
        return false;
    } else {
        QQuickItemPrivate *f = QQuickItemPrivate::get(item);
        if (f == Q_NULLPTR) {
            //don't fall apart, but give a warning
            qDebug() << "ScrollbarUtils: failed to get item's pimpl.";
            return false;
        }
        return f->componentComplete;
    }
}

UT_NAMESPACE_END
