#include "ucscrollbarutils.h"
#include <QtQuick/private/qquickflickable_p.h>

UCScrollbarUtils::UCScrollbarUtils(QObject *parent) : QObject(parent)
{
}

//HACK: to be able to know if a Flickable is "complete" or not.
//That info is not exposed to QML. You only get the Component::completed()
//signal but if you want to check if a Flickable you get is already
//complete (hence the signal will never come) the only way is to call
//QQuickItem::isComponentComplete() which, as of Qt5.7, is c++ "protected".
//We need this hack to be able to call that method.
class UCFlickableFriend : public QQuickFlickable
{
    friend class UCScrollbarUtils;
};
bool UCScrollbarUtils::isFlickableComplete(QQuickFlickable *flickable)
{
    if (flickable == Q_NULLPTR) {
        qDebug() << "ScrollbarUtils: null Flickable parameter.";
        return false;
    } else {
        UCFlickableFriend *f = reinterpret_cast<UCFlickableFriend *>(flickable);
        if (f == Q_NULLPTR) {
            //don't fall apart, but give a warning
            qDebug() << "ScrollbarUtils: Flickable cast failure.";
            return false;
        }
        return f->isComponentComplete();
    }
}
