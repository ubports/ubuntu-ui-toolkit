/*
 * Copyright 2015 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#include "ucstyleditembase.h"
#include "ucstyleditembase_p.h"
#include "uctheme.h"

UCStyledItemBasePrivate::UCStyledItemBasePrivate()
    : activeFocusOnPress(false)
    , subthemingEnabled(true)
    , theme(0)
    , parentStyledItem(0)
{
}

UCStyledItemBasePrivate::~UCStyledItemBasePrivate()
{
}

void UCStyledItemBasePrivate::init()
{
    Q_Q(UCStyledItemBase);
    q->setFlag(QQuickItem::ItemIsFocusScope);
    QByteArray env = qgetenv("UITK_SUBTHEMING");
    subthemingEnabled = (env.isEmpty() || env == QByteArrayLiteral("yes"));
    if (!subthemingEnabled) {
        // every theme will be using the default theme
        theme = &UCTheme::defaultTheme();
        QObject::connect(theme, &UCTheme::nameChanged, q, &UCStyledItemBase::themeChanged);
    } else {
        QObject::connect(&UCTheme::defaultTheme(), &UCTheme::nameChanged,
                         q, &UCStyledItemBase::themeChanged);
    }
}


void UCStyledItemBasePrivate::setFocusable(bool focus)
{
    Q_Q(UCStyledItemBase);
    q->setAcceptedMouseButtons(focus ? (Qt::LeftButton | Qt::RightButton | Qt::MiddleButton) : Qt::NoButton);
    q->setFiltersChildMouseEvents(focus);
}

bool UCStyledItemBasePrivate::isParentFocusable()
{
    if (!activeFocusOnPress) {
        return false;
    }
    QQuickItem *pl = parentItem;
    while (pl) {
        UCStyledItemBase *scope = qobject_cast<UCStyledItemBase*>(pl);
        if (scope) {
            UCStyledItemBasePrivate *pscope = UCStyledItemBasePrivate::get(scope);
            return pscope->isParentFocusable();
        }
        pl = pl->parentItem();
    }

    return true;
}


/*!
 * \qmltype StyledItemBase
 * \instantiates UCStyledItemBase
 * \inqmlmodule Ubuntu.Components 1.1
 * \ingroup ubuntu
 * \since Ubuntu.Components 1.1
 * \internal
 * \qmlabstract
 */
UCStyledItemBase::UCStyledItemBase(QQuickItem *parent)
    : QQuickItem(*(new UCStyledItemBasePrivate), parent)
{
    Q_D(UCStyledItemBase);
    d->init();
}

UCStyledItemBase::UCStyledItemBase(UCStyledItemBasePrivate &dd, QQuickItem *parent)
    : QQuickItem(dd, parent)
{
    Q_D(UCStyledItemBase);
    d->init();
}

/*!
 * \qmlmethod void StyledItemBase::requestFocus(Qt::FocusReason reason)
 * \since Ubuntu.Components 1.1
 * The function is similar to \c forceActiveFocus except that it sets focus only
 * if all the ancestors have activeFocusOnPressed set. Returns true if the request
 * succeeded.
 */
bool UCStyledItemBase::requestFocus(Qt::FocusReason reason)
{
    Q_D(UCStyledItemBase);
    bool focusable = d->isParentFocusable();
    if (focusable && isEnabled()) {
        QQuickItem::forceActiveFocus(reason);
    }
    return focusable;
}

/*!
 * \qmlproperty bool StyledItemBase::activeFocusOnPress
 * \since Ubuntu.Components 1.1
 *
 * The property specifies whether the StyledItem can gain focus on a mouse
 * press/touch or not. When the value is true, the focus will be set on the component
 * when the mouse is pressed over it or touched. However if one of the component's
 * ancestor StyledItem or derived is having the property value false, the focus
 * will not be gained automatically.
 *
 * In the following example the TextField will stay focused when clicked on the
 * red rectangle.
 * \qml
 * import QtQuick 2.4
 * import Ubuntu.Components 1.2
 *
 * Column {
 *     width: units.gu(50)
 *     height: units.gu(100)
 *
 *     StyledItem {
 *         objectName: "passiveScope"
 *         width: parent.width
 *         height: units.gu(30)
 *         Rectangle {
 *             anchors.fill: parent
 *             color: "red"
 *             StyledItem {
 *                 objectName: "activeScope"
 *                 activeFocusOnPress: true
 *                 anchors.fill: parent
 *             }
 *         }
 *     }
 *
 *     TextField {
 *         id: input
 *         text: "The input stays focus even if red box is clicked"
 *     }
 *
 *     Component.onCompleted: input.forceActiveFocus()
 *
 *     Connections {
 *         target: window
 *         onActiveFocusItemChanged: console.debug("focus on", window.activeFocusItem)
 *     }
 * }
 * \endqml
 *
 * The default value is \c false.
 */
bool UCStyledItemBase::activefocusOnPress() const
{
    Q_D(const UCStyledItemBase);
    return d->activeFocusOnPress;
}
void UCStyledItemBase::setActiveFocusOnPress(bool value)
{
    Q_D(UCStyledItemBase);
    if (d->activeFocusOnPress == value)
        return;
    d->activeFocusOnPress = value;
    d->setFocusable(d->activeFocusOnPress);
    Q_EMIT activeFocusOnPressChanged();
}

/*!
 * \qmlproperty ThemeSettings StyledItem::theme
 * \since Ubuntu.Components 1.3
 * The property configures the theme the component and all its sub-components
 * will use. By default it is set to the closest ancestor StyledItem's theme
 * if any, or to the system default theme.
 */
UCTheme *UCStyledItemBasePrivate::getTheme() const
{
    if (subthemingEnabled) {
        if (theme) {
            return theme;
        } else if (!parentStyledItem.isNull()) {
            return UCStyledItemBasePrivate::get(parentStyledItem)->getTheme();
        }
    }
    return &UCTheme::defaultTheme();
}
void UCStyledItemBasePrivate::setTheme(UCTheme *newTheme)
{
    Q_Q(UCStyledItemBase);
    if (theme == newTheme) {
        return;
    }

    if (!subthemingEnabled) {
        // no subtheming
        if (newTheme) {
            theme = newTheme;
            UCTheme::defaultTheme().setName(theme->name());
        } else {
            UCTheme::defaultTheme().resetName();
        }
        return;
    }

    // disconnect from the previous set
    UCTheme *connectedSet = theme ?
                                theme :
                                (!parentStyledItem ? &UCTheme::defaultTheme() : NULL);
    if (connectedSet) {
        QObject::disconnect(connectedSet, &UCTheme::nameChanged,
                            q, &UCStyledItemBase::themeChanged);
    }

    UCTheme *prevSet = theme;

    // resolve new theme
    if (theme && newTheme) {
        // no need to redo the parentStack, simply set the theme and leave
        theme = newTheme;
    } else {
        theme = newTheme;
        if (!newTheme) {
            // redo the parent chanin
            disconnectTillItem(0);
            connectParents(0);
        }
    }

    // connect to the new set
    connectedSet = theme ?
                    theme :
                    (!parentStyledItem ? &UCTheme::defaultTheme() : NULL);
    if (connectedSet) {
        QObject::connect(connectedSet, &UCTheme::nameChanged,
                         q, &UCStyledItemBase::themeChanged);
    }
    // detach previous set and attach the new one
    if (prevSet) {
        Q_EMIT prevSet->parentThemeChanged();
    }
    if (theme) {
        // re-parent theme to make sure we have it
        // for the entire lifetime of the styled item
        theme->setParent(q);
        Q_EMIT theme->parentThemeChanged();
    }

    Q_EMIT q->themeChanged();
}
void UCStyledItemBasePrivate::resetTheme()
{
    setTheme(NULL);
}

// link/unlink all ascendant items until we reach a StyledItem, returns true if the
// theme change signal emission is justified
bool UCStyledItemBasePrivate::connectParents(QQuickItem *fromItem)
{
    Q_Q(UCStyledItemBase);
    QQuickItem *item = fromItem ? fromItem : parentItem;
    while (item) {
        // push the item onto the stack
        parentStack.push(QPointer<QQuickItem>(item));
        UCStyledItemBase *styledItem = qobject_cast<UCStyledItemBase*>(item);
        if (styledItem) {
            // this is the closest StyledItem, connect its themeChanged() signal
            QObject::connect(styledItem, SIGNAL(themeChanged()),
                             q, SLOT(_q_parentStyleChanged()), Qt::DirectConnection);
            // set the current style set to the one in the parent's one if differs
            return setParentStyled(styledItem);
        } else {
            // connect to the item's parentChanged() signal so we can detect when the parent changes
            QObject::connect(item, SIGNAL(parentChanged(QQuickItem*)),
                             q, SLOT(_q_ascendantChanged(QQuickItem*)), Qt::DirectConnection);
        }
        item = item->parentItem();
    }
    return false;
}

// set the used parent styled item's style; returns true if the parent styled got changed
bool UCStyledItemBasePrivate::setParentStyled(UCStyledItemBase *styledItem)
{
    if (parentStyledItem == styledItem) {
        return false;
    }
    parentStyledItem = styledItem;
    if (theme) {
        Q_EMIT theme->parentThemeChanged();
    }
    return (theme == NULL);
}

// disconnect parent stack till item is reached; all the stack if item == 0
void UCStyledItemBasePrivate::disconnectTillItem(QQuickItem *item)
{
    if (!subthemingEnabled) {
        return;
    }
    Q_Q(UCStyledItemBase);
    while (!parentStack.isEmpty() && item != parentStack.top()) {
        QPointer<QQuickItem> stackItem = parentStack.pop();
        // the topmost item can be the only one which is a StyledItem
        UCStyledItemBase *styledItem = qobject_cast<UCStyledItemBase*>(stackItem.data());
        if (styledItem) {
            QObject::disconnect(styledItem, SIGNAL(themeChanged()),
                                q, SLOT(_q_parentStyleChanged()));
        } else if (!stackItem.isNull()) {
            QObject::disconnect(stackItem.data(), SIGNAL(parentChanged(QQuickItem*)),
                                q, SLOT(_q_ascendantChanged(QQuickItem*)));
        }
    }
}

// captures ascendant change signal, the sender is the one which counts!
void UCStyledItemBasePrivate::_q_ascendantChanged(QQuickItem *ascendant)
{
    Q_Q(UCStyledItemBase);
    QQuickItem *sender = static_cast<QQuickItem*>(q->sender());
    if (!sender) {
        // cannot detect the sender, leave!
        return;
    }
    if (ascendant) {
        // disconnect from the previous ones
        disconnectTillItem(sender);
        // traverse ascendants till we reach a StyledItem or root and push them into the stack
        if (connectParents(ascendant)) {
            Q_EMIT q->themeChanged();
        }
    }
}

// syncs the ascendant StyledItem's style
void UCStyledItemBasePrivate::_q_parentStyleChanged()
{
    // do not trigger themeChanged() on this item if we have a
    // custom one, but resolve its eventual parent change!
    if (theme) {
        Q_EMIT theme->parentThemeChanged();
        return;
    }
    Q_Q(UCStyledItemBase);
    UCStyledItemBase *styledItem = static_cast<UCStyledItemBase*>(q->sender());
    if (!styledItem) {
        return;
    }
    setParentStyled(styledItem);
    Q_EMIT q->themeChanged();
}

// grab pressed state and focus if it can be
void UCStyledItemBase::mousePressEvent(QMouseEvent *event)
{
    QQuickItem::mousePressEvent(event);
    requestFocus(Qt::MouseFocusReason);
}

// filter children events as well, so we can catch mouse presses done over child
// MouseAreas or other mouse grabbers
bool UCStyledItemBase::childMouseEventFilter(QQuickItem *child, QEvent *event)
{
    // only filter pressed events
    if (event->type() == QEvent::MouseButtonPress) {
        // send mouse event
        QMouseEvent *mouse = static_cast<QMouseEvent*>(event);
        // the event may occur outside of the parent's boundaries if not clipped
        // therefore must check containment
        QPointF point = mapFromItem(child, mouse->localPos());
        if (contains(point)) {
            requestFocus(Qt::MouseFocusReason);
        }
    }
    // let the event be passed to children
    return QQuickItem::childMouseEventFilter(child, event);
}

// catch parent change event so we can lookup for the parent chain theme
void UCStyledItemBase::itemChange(ItemChange change, const ItemChangeData &data)
{
    QQuickItem::itemChange(change, data);
    Q_D(UCStyledItemBase);
    if (change == ItemParentHasChanged && d->subthemingEnabled) {
        if (!data.item) {
            d->disconnectTillItem(0);
        } else if (d->connectParents(0)) {
            Q_EMIT themeChanged();
        }
    }
}

#include "moc_ucstyleditembase.cpp"
