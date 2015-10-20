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

#include "ucthemingextension.h"
#include "uctheme.h"
#include <QtQuick/private/qquickitem_p.h>
#include <QtQml/private/qqmlcomponentattached_p.h>
#include <QtGui/QGuiApplication>

/*
 * The UCThemingExtension class provides theme handling on Items extending an existing
 * QQuickItem derivate class. Items subject fo theming should derive from this class
 * and implement the virtual methods, as well as add the following two methods:
 * 1) classBegin(item) should be called from the QQuickItem::classBegin() method
 * 2) handleThemeEvent() should be called from QQuickItem::customEvent() method
 * The item can expose the theme property and use the getters defined by the
 * class.
 * Iin case the item exposes the theme property, it can use the getters from the
 * extension and declare the themeChanged signal.
 */

static int themeUpdatedId = QEvent::registerEventType();
static int themeReloadedId = QEvent::registerEventType();

UCThemeEvent::UCThemeEvent(UCTheme *reloadedTheme)
    : QEvent((QEvent::Type)themeReloadedId)
    , m_oldTheme(Q_NULLPTR)
    , m_newTheme(reloadedTheme)
{
    setAccepted(false);
}

UCThemeEvent::UCThemeEvent(UCTheme *oldTheme, UCTheme *newTheme)
    : QEvent((QEvent::Type)themeUpdatedId)
    , m_oldTheme(oldTheme)
    , m_newTheme(newTheme)
{
    setAccepted(false);
}

UCThemeEvent::UCThemeEvent(const UCThemeEvent &other)
    : QEvent(other.type())
    , m_oldTheme(other.m_oldTheme)
    , m_newTheme(other.m_newTheme)
{
    setAccepted(false);
}

bool UCThemeEvent::isThemeEvent(const QEvent *event)
{
    return ((int)event->type() == themeUpdatedId) || ((int)event->type() == themeReloadedId);
}

void UCThemingExtension::forwardEvent(QQuickItem *item, UCThemeEvent *event)
{
    Q_FOREACH(QQuickItem *child, item->childItems()) {
        QGuiApplication::sendEvent(child, event);
        // StyledItem will handle the broadcast itself depending on whether the theme change was appropriate or not
        // and will complete the ascendantStyled/theme itself
        if (child->childItems().size() > 0 && !isThemed(child)) {
            forwardEvent(child, event);
        }
    }
}

void UCThemingExtension::broadcastThemeChange(QQuickItem *item, UCTheme *oldTheme, UCTheme *newTheme)
{
    UCThemeEvent event(oldTheme, newTheme);
    forwardEvent(item, &event);
}

void UCThemingExtension::broadcastThemeReloaded(QQuickItem *item, UCTheme *theme)
{
    UCThemeEvent event(theme);
    forwardEvent(item, &event);
}

/*************************************************************************
 * Attached to every Item in the system
 */
static uint xdata = QObject::registerUserData();
UCItemAttached::UCItemAttached(QQuickItem *owner)
    : m_item(owner)
    , m_prevParent(Q_NULLPTR)
{
    QQuickItemPrivate::get(m_item)->addItemChangeListener(this, QQuickItemPrivate::Parent);
}

UCItemAttached::~UCItemAttached()
{
    QQuickItemPrivate::get(m_item)->removeItemChangeListener(this, QQuickItemPrivate::Parent);
}

bool UCThemingExtension::isThemed(QQuickItem *item)
{
    UCThemingExtension *extension = qobject_cast<UCThemingExtension*>(item);
    return extension != Q_NULLPTR;
}

// handle parent changes
void UCItemAttached::itemParentChanged(QQuickItem *, QQuickItem *newParent)
{
    if (newParent == m_prevParent || QQuickItemPrivate::get(m_item)->wasDeleted) {
        return;
    }

    // make sure we have these handlers attached to each intermediate item
    QQuickItem *oldThemedAscendant = UCThemingExtension::ascendantThemed(m_prevParent);
    QQuickItem *newThemedAscendant = UCThemingExtension::ascendantThemed(newParent);
    UCTheme *oldTheme = oldThemedAscendant ? oldThemedAscendant->property("theme").value<UCTheme*>() : &UCTheme::defaultTheme();
    UCTheme *newTheme = newThemedAscendant ? newThemedAscendant->property("theme").value<UCTheme*>() : &UCTheme::defaultTheme();

    if (oldTheme != newTheme) {
        UCThemingExtension *extension = qobject_cast<UCThemingExtension*>(m_item);
        // send the event to m_item first
        UCThemeEvent event(oldTheme, newTheme);
        if (extension) {
            // only items with extensions should get this event
            extension->handleThemeEvent(&event);
        }
        // then broadcast to children, but only if the item is not a styled one
        if (!extension) {
            UCThemingExtension::forwardEvent(m_item, &event);
        }
    }
    m_prevParent = newParent;
}

/*************************************************************************
 *
 */
UCThemingExtension::UCThemingExtension(QQuickItem *extendedItem)
    : theme(&UCTheme::defaultTheme())
    , themedItem(extendedItem)
    , themeType(Inherited)
{
    theme->attachItem(themedItem, true);
    themedItem->setUserData(xdata, new UCItemAttached(themedItem));
}

UCThemingExtension::~UCThemingExtension()
{
    if (theme) {
        theme->attachItem(themedItem, false);
    }
}

// set the parent of the theme if the themeType is Custom
void UCThemingExtension::setParentTheme()
{
    if (themeType != Custom) {
        return;
    }
    QQuickItem *upperThemed = ascendantThemed(QQuickItemPrivate::get(themedItem)->parentItem);
    UCThemingExtension *extension = qobject_cast<UCThemingExtension*>(upperThemed);
    UCTheme *parentTheme = extension ? extension->getTheme() : &UCTheme::defaultTheme();
    if (parentTheme != theme) {
        theme->setParentTheme(parentTheme);
    }
}

void UCThemingExtension::handleThemeEvent(UCThemeEvent *event)
{
    if ((int)event->type() == themeUpdatedId) {
        switch (themeType) {
        case Inherited: {
            setTheme(event->newTheme(), Inherited);
            return;
        }
        case Custom: {
            // set the theme's parent
            theme->setParentTheme(event->newTheme());
            return;
        }
        default: break;
        }
    } else if ((int)event->type() == themeReloadedId) {
        // no need to handle Inherited case, those items are all attached to the theme changes
        switch (themeType) {
        case Inherited: {
            preThemeChanged();
            postThemeChanged();
            return;
        }
        case Custom: {
            if (event->newTheme() == theme) {
                preThemeChanged();
                postThemeChanged();
                // forward to children
                forwardEvent(themedItem, event);
            } else {
                // emit theme's parentThemeChanged()
                Q_EMIT theme->parentThemeChanged();
            }
            return;
        }
        default: break;
        }
    }
}

UCTheme *UCThemingExtension::getTheme() const
{
    return theme;
}
void UCThemingExtension::setTheme(UCTheme *newTheme, ThemeType type)
{
    if (theme == newTheme) {
        return;
    }

    // preform pre-theme change tasks
    preThemeChanged();

    themeType = type;
    UCTheme *oldTheme = theme;

    // disconnect from the previous set
    if (theme) {
        theme->attachItem(themedItem, false);
    }

    theme = newTheme;

    // connect to the new set
    if (theme) {
        theme->attachItem(themedItem, true);
        // set the parent of the theme if custom
        setParentTheme();
    }

    // perform post-theme changes, update internal styling
    postThemeChanged();

    // broadcast to the children
    broadcastThemeChange(themedItem, oldTheme, theme);
}

void UCThemingExtension::resetTheme()
{
    QQuickItem *upperThemed = ascendantThemed(QQuickItemPrivate::get(themedItem)->parentItem);
    UCThemingExtension *extension = qobject_cast<UCThemingExtension*>(upperThemed);
    UCTheme *theme = extension ? extension->getTheme() : &UCTheme::defaultTheme();
    setTheme(theme, Inherited);
}

// returns the closest themed ascendant
QQuickItem *UCThemingExtension::ascendantThemed(QQuickItem *item)
{
    while (item && !isThemed(item)) {
        // if the item has no xdata set, means we haven't been here yet
        if (!item->userData(xdata)) {
            item->setUserData(xdata, new UCItemAttached(item));
        }
        item = item->parentItem();
    }
    return item;
}
