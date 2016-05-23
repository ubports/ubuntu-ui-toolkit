#include "ucscrollbar.h"
#include "ucscrollbar_p.h"
#include <QtQuick/private/qquickflickable_p.h>

UCScrollbarPrivate::UCScrollbarPrivate()
    : UCStyledItemBasePrivate()
    , flickableItem(Q_NULLPTR)
    , alignment(UCScrollbar::AlignTrailing)
    , buddyScrollbar(Q_NULLPTR)
    , viewport(Q_NULLPTR)
    , alwaysOnScrollbar(false)
    , interactive(true)
{
}

UCScrollbarPrivate::~UCScrollbarPrivate()
{
}

void UCScrollbarPrivate::init()
{
    styleDocument = QStringLiteral("ScrollbarStyle");
}

UCScrollbar::UCScrollbar(QQuickItem *parent) :
    UCStyledItemBase(*(new UCScrollbarPrivate), parent)
{
    Q_D(UCScrollbar);
    d->init();
}

QQuickFlickable *UCScrollbar::flickableItem()
{
    Q_D(UCScrollbar);
    return d->flickableItem;
}

void UCScrollbar::setFlickableItem(QQuickFlickable *item)
{
    Q_D(UCScrollbar);
    if (d->flickableItem == item) return;

    d->flickableItem = item;
    Q_EMIT flickableItemChanged();
}

UCScrollbar::UCScrollbarAlignment UCScrollbar::align()
{
    Q_D(UCScrollbar);
    return d->alignment;
}

void UCScrollbar::setAlign(UCScrollbarAlignment val)
{
    Q_D(UCScrollbar);
    if (d->alignment == val) return;

    d->alignment = val;
    Q_EMIT alignChanged();
}

UCScrollbar *UCScrollbar::buddyScrollbar()
{
    Q_D(UCScrollbar);
    return d->buddyScrollbar;
}
void UCScrollbar::setBuddyScrollbar(UCScrollbar *buddy)
{
    Q_D(UCScrollbar);
    if (d->buddyScrollbar == buddy) return;

    d->buddyScrollbar = buddy;
    Q_EMIT buddyScrollbarChanged();
}

QQuickItem *UCScrollbar::viewport()
{
    Q_D(UCScrollbar);
    return d->viewport;
}
void UCScrollbar::setViewport(QQuickItem *viewport)
{
    Q_D(UCScrollbar);
    if (d->viewport == viewport) return;

    d->viewport = viewport;
    Q_EMIT viewportChanged();
}

bool UCScrollbar::alwaysOnScrollbar()
{
    Q_D(UCScrollbar);
    return d->alwaysOnScrollbar;
}
void UCScrollbar::setAlwaysOnScrollbar(bool val)
{
    Q_D(UCScrollbar);
    if (d->alwaysOnScrollbar == val) return;

    d->alwaysOnScrollbar = val;
    Q_EMIT alwaysOnScrollbarChanged();
}

bool UCScrollbar::interactive()
{
    Q_D(UCScrollbar);
    return d->interactive;
}
void UCScrollbar::setInteractive(bool val)
{
    Q_D(UCScrollbar);
    if (d->interactive == val) return;

    d->interactive = val;
    Q_EMIT interactiveChanged();
}



#include "moc_ucscrollbar.cpp"
