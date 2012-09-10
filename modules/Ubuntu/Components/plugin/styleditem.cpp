#include "styleditem.h"
#include "styleditem_p.h"
#include "style.h"
#include "themeengine.h"

#include <QDeclarativeProperty>

StyledItemPrivate::StyledItemPrivate(StyledItem *qq):
    q_ptr(qq),
    useCustomStyle(false),
    activeStyle(0)
{
}

void StyledItemPrivate::setUid(const QString &newUid)
{
    Q_Q(StyledItem);

    if (uid != newUid) {
        // lookup in between components to see whether this UID is in use
    }
}

void StyledItemPrivate::_q_updateCurrentStyle(const QString &state)
{
    qDebug() << styleClass << state;
    // updates the style based on the current state
    Style *tmp = 0;
    if (useCustomStyle) {
        // TODO: implement lookup
    } else {
        tmp = ThemeEngine::lookupStateStyle(styleClass, state);
        if (!tmp) {
            //qDebug() << "\tno style for" << styleClass;
            // check if the styleClass is a compound one and try to find a style that matches
            QString sclass(styleClass);
            while (!tmp && (sclass.indexOf('.') >= 0)) {
                // remove the first StyledItem class name from the block
                sclass = sclass.right(sclass.length()-sclass.indexOf('.') - 1);
                //qDebug() << "\tlookup for" << sclass;
                // and search for the remained style-path
                tmp = ThemeEngine::lookupStateStyle(sclass, state);
            }
        }
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
    QObject::connect(this, SIGNAL(stateChanged(const QString &)), this, SLOT(_q_updateCurrentStyle(const QString &)));
}

StyledItem::~StyledItem()
{}

void StyledItem::componentComplete()
{
    Q_D(StyledItem);
    d->useCustomStyle = (d->customStyles.size() > 0);

    // fetch the current componet's ID
    qDebug() << "ID" << QDeclarativeProperty::read(this, "id").toString();

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
        qDebug() << "styleClass::" << d->styleClass;
    }

    // set the default style
    d->_q_updateCurrentStyle("");
}

QString StyledItem::uid() const
{
    Q_D(const StyledItem);
    return d->uid;
}
void StyledItem::setUid(const QString &uid)
{
    Q_D(StyledItem);
    d->setUid(uid);
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
