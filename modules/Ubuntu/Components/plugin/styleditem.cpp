#include "styleditem.h"
#include "styleditem_p.h"
#include "style.h"
#include "themeengine.h"

StyledItemPrivate::StyledItemPrivate(StyledItem *qq):
    q_ptr(qq),
    useCustomStyle(false),
    activeStyle(0)
{
}

void StyledItemPrivate::_q_updateCurrentStyle(const QString &state)
{
    // updates the style based on the current state
    Style *tmp = 0;
    if (useCustomStyle) {
        // TODO: implement lookup
    } else {
        tmp = ThemeEngine::stateStyle(styleClass, state);
    }
    if ((tmp != activeStyle) && tmp) {
        Q_Q(StyledItem);
        activeStyle = tmp;
        emit q->styleChanged();
    }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
StyledItem::StyledItem(QDeclarativeItem *parent) :
    QDeclarativeItem(parent),
    d_ptr(new StyledItemPrivate(this))
{
    QObject::connect(this, SIGNAL(stateChanged(QString)), this, SLOT(_q_updateCurrentStyle(QString)));
}

StyledItem::~StyledItem()
{}

void StyledItem::componentComplete()
{
    Q_D(StyledItem);
    d->useCustomStyle = (d->customStyles.size() > 0);

    if (!d->useCustomStyle) {
        // Get parent paths. Consider QMLTYPES only, skip every QDeclarative component
        d->styleClass = metaObject()->className();
        d->styleClass = d->styleClass.left(d->styleClass.indexOf("_QMLTYPE"));
        QDeclarativeItem *pl = parentItem();
        while (pl) {
            if (qobject_cast<StyledItem*>(pl)) {
                QString plClass = pl->metaObject()->className();
                plClass = plClass.left(plClass.indexOf("_QMLTYPE"));
                d->styleClass.prepend('.');
                d->styleClass.prepend(plClass);
            }
            pl = qobject_cast<QDeclarativeItem*>(pl)->parentItem();
        }
    }

    // set the default style
    d->_q_updateCurrentStyle("");
}

Style *StyledItem::activeStyle() const
{
    Q_D(const StyledItem);
    return d->activeStyle;
}

QDeclarativeListProperty<Style> StyledItem::customStyles()
{
    Q_D(StyledItem);
    return QDeclarativeListProperty<Style>(this, d->customStyles);
}

#include "moc_styleditem.cpp"
