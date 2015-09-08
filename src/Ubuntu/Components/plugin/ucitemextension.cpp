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
 * handle the theme change can therefore catch this event by extending the QObject::event()
 * method and handle the event there.
 */

int UCItemExtension::ascendantChangedEvent = 0;
UCThemeUpdateEvent::UCThemeUpdateEvent(UCStyledItemBase *newStyled, UCTheme *newTheme)
    : QEvent((QEvent::Type)UCItemExtension::ascendantChangedEvent)
    , m_ascendantStyled(newStyled)
    , m_theme(newTheme)
{
}

UCItemExtension::UCItemExtension(QObject *parent)
    : QObject(parent)
    , m_item(static_cast<QQuickItem*>(parent))
    , m_completed(false)
{
    connect(m_item, &QQuickItem::parentChanged, this, &UCItemExtension::extendedParentChanged);
    // listen for completion
    QQmlComponentAttached *attached = QQmlComponent::qmlAttachedProperties(m_item);
    connect(attached, &QQmlComponentAttached::completed, this, &UCItemExtension::completed);
    // get parent item changes
    connect(m_item, &QQuickItem::parentChanged, this, &UCItemExtension::handleParentChanged);
}

// get completion
void UCItemExtension::completed()
{
    m_completed = true;
}

// handle parent changes
void UCItemExtension::handleParentChanged(QQuickItem *newParent)
{
    // traverse new parent ascendants till we reach a styled item and get the theme of that one
    QQuickItem *pl = newParent;
    UCStyledItemBase *styledItem = qobject_cast<UCStyledItemBase*>(pl);
    while (pl && !styledItem) {
        pl = pl->parentItem();
        styledItem = qobject_cast<UCStyledItemBase*>(pl);
    }
    UCTheme *theme = styledItem ?
                UCStyledItemBasePrivate::get(styledItem)->getTheme() :
                &UCTheme::defaultTheme();
    broadcastParentChange(m_item, styledItem, theme);
}

// boadcast parent change to the children recoursively
void UCItemExtension::broadcastParentChange(QQuickItem *item, UCStyledItemBase *ascendantStyled, UCTheme *newTheme)
{
    Q_FOREACH(QQuickItem *child, item->childItems()) {
        QGuiApplication::postEvent(child, new UCThemeUpdateEvent(ascendantStyled, newTheme));
        if (child->childItems().size() > 0) {
            broadcastParentChange(child, ascendantStyled, newTheme);
        }
    }
}

QQuickItem *UCItemExtension::parentItem() const
{
    return m_item->parentItem();
}
void UCItemExtension::setParentItem(QQuickItem *parentItem)
{
    m_item->setParentItem(parentItem);
}
