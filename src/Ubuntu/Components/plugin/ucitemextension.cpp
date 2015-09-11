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

#include "ucitemextension.h"
#include "ucstyleditembase_p.h"
#include "uctheme.h"
#include <QtQml/private/qqmlcomponentattached_p.h>
#include <QtGui/QGuiApplication>

/*
 * The UCItemExtension class provides faster theme change broadcast and saves us from
 * the need to connect to ascendant instances all around. It is meant to extend QQuickItem
 * (see plugin.cpp registration) therefore it extends any QtQuick element.
 * Whenever the extended Item's parentItem is changed, there will be an UCThemeUpdateEvent
 * event broadcasted to all item's children and their children. Components which need to
 * handle the theme change can therefore catch this event by extending the QObject::customEvent()
 * method and handle the event there.
 */

//#define ASYNC_BROADCAST
int UCThemeUpdateEvent::themeEventId = QEvent::registerEventType();
UCThemeUpdateEvent::UCThemeUpdateEvent(UCTheme *oldTheme, UCTheme *newTheme)
    : QEvent((QEvent::Type)UCThemeUpdateEvent::themeEventId)
    , m_oldTheme(oldTheme)
    , m_newTheme(newTheme)
{
}

void UCThemeUpdateEvent::handleEvent(QQuickItem *item, UCTheme *oldTheme, UCTheme *newTheme)
{
#ifdef ASYNC_BROADCAST
        QGuiApplication::postEvent(item, new UCThemeUpdateEvent(oldTheme, newTheme));
#else
        UCThemeUpdateEvent event(oldTheme, newTheme);
        QGuiApplication::sendEvent(item, &event);
#endif
}

void UCThemeUpdateEvent::broadcastToChildren(QQuickItem *item, UCTheme *oldTheme, UCTheme *newTheme)
{
    Q_FOREACH(QQuickItem *child, item->childItems()) {
        handleEvent(child, oldTheme, newTheme);
        // StyledItem will handle the broadcast itself depending on whether the theme change was appropriate or not
        // and will complete the ascendantStyled/theme itself
        if (child->childItems().size() > 0 && !qobject_cast<UCStyledItemBase*>(child)) {
            broadcastToChildren(child, oldTheme, newTheme);
        }
    }
}

void UCThemeUpdateEvent::forwardEvent(QQuickItem *item, UCThemeUpdateEvent *event)
{
    broadcastToChildren(item, event->oldTheme(), event->newTheme());
}

/*************************************************************************
 *
 */
UCItemExtension::UCItemExtension(QObject *parent)
    : QObject(parent)
    , m_item(static_cast<QQuickItem*>(parent))
    , m_prevParent(Q_NULLPTR)
{
    // we have the parent ready when we get in here, the extension is created first time the parent property is invoked
    connect(m_item, &QQuickItem::parentChanged, this, &UCItemExtension::extendedParentChanged);
    // get parent item changes
    connect(m_item, &QQuickItem::parentChanged, this, &UCItemExtension::handleParentChanged);
    // handle parent changes right away
    handleParentChanged(m_item->parentItem());
}

// handle parent changes
void UCItemExtension::handleParentChanged(QQuickItem *newParent)
{
    if (newParent == m_prevParent) {
        // shouldn't be ever encountered, but for safety...
        return;
    }
    UCStyledItemBase *oldStyledAscendant = UCStyledItemBase::ascendantStyled(m_prevParent);
    UCStyledItemBase *newStyledAscendant = UCStyledItemBase::ascendantStyled(newParent);
    UCTheme *oldTheme = oldStyledAscendant ? UCStyledItemBasePrivate::get(oldStyledAscendant)->getTheme() : &UCTheme::defaultTheme();
    UCTheme *newTheme = newStyledAscendant ? UCStyledItemBasePrivate::get(newStyledAscendant)->getTheme() : &UCTheme::defaultTheme();

    if (oldTheme != newTheme) {
        // send the event to m_item first
        UCThemeUpdateEvent::handleEvent(m_item, oldTheme, newTheme);
        // then broadcast to children, but only if the item is not a styled one
        if (!qobject_cast<UCStyledItemBase*>(m_item)) {
            UCThemeUpdateEvent::broadcastToChildren(m_item, oldTheme, newTheme);
        }
    }
    m_prevParent = newParent;
}

// property overrides forward to the original the getter/setter
QQuickItem *UCItemExtension::parentItem() const
{
    return m_item->parentItem();
}
void UCItemExtension::setParentItem(QQuickItem *parentItem)
{
    m_item->setParentItem(parentItem);
}
