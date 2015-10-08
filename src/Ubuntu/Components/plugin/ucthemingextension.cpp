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
        if (child->childItems().size() > 0 && !UCItemAttached::isThemed(child)) {
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
UCItemAttached::UCItemAttached(QObject *owner)
    : QObject(owner)
    , m_item(static_cast<QQuickItem*>(owner))
    , m_prevParent(Q_NULLPTR)
    , m_extension(Q_NULLPTR)
{
    // get parent item changes
    connect(m_item, &QQuickItem::parentChanged, this, &UCItemAttached::handleParentChanged);
}

UCItemAttached::~UCItemAttached()
{
}

UCItemAttached *UCItemAttached::qmlAttachedProperties(QObject *owner)
{
    return new UCItemAttached(owner);
}

bool UCItemAttached::isThemed(QQuickItem *item)
{
    UCItemAttached *attached = static_cast<UCItemAttached*>(qmlAttachedPropertiesObject<UCItemAttached>(item, false));
    return attached && (attached->m_extension != Q_NULLPTR);
}

// handle parent changes
void UCItemAttached::handleParentChanged(QQuickItem *newParent)
{
    if (newParent == m_prevParent || QQuickItemPrivate::get(m_item)->wasDeleted) {
        return;
    }

    // make sure we have these handlers attached to each intermediate item
    QQuickItem *oldThemedAscendant = UCThemingExtension::ascendantThemed(m_prevParent);
    QQuickItem *newThemedAscendant = UCThemingExtension::ascendantThemed(newParent);
    UCItemAttached *oldAscendantAttached = static_cast<UCItemAttached*>(qmlAttachedPropertiesObject<UCItemAttached>(oldThemedAscendant, false));
    UCItemAttached *newAscendantAttached = static_cast<UCItemAttached*>(qmlAttachedPropertiesObject<UCItemAttached>(newThemedAscendant, false));
    UCThemingExtension *oldAscendantExtension = oldAscendantAttached ? oldAscendantAttached->m_extension : Q_NULLPTR;
    UCThemingExtension *newAscendantExtension = newAscendantAttached ? newAscendantAttached->m_extension : Q_NULLPTR;
    UCTheme *oldTheme = oldAscendantExtension ? oldAscendantExtension->getTheme() : &UCTheme::defaultTheme();
    UCTheme *newTheme = newAscendantExtension ? newAscendantExtension->getTheme() : &UCTheme::defaultTheme();

    if (oldTheme != newTheme) {
        // send the event to m_item first
        UCThemeEvent event(oldTheme, newTheme);
        if (m_extension) {
            // only items with extensions should get this event
            m_extension->handleThemeEvent(&event);
        }
        // then broadcast to children, but only if the item is not a styled one
        if (!m_extension) {
            UCThemingExtension::forwardEvent(m_item, &event);
        }
    }
    m_prevParent = newParent;
}

void UCItemAttached::reloadTheme()
{
    Q_ASSERT(m_extension);
    m_extension->preThemeChanged();
    m_extension->postThemeChanged();
    UCThemingExtension::broadcastThemeReloaded(m_item, static_cast<UCTheme*>(sender()));
}

/*************************************************************************
 *
 */
UCThemingExtension::UCThemingExtension()
    : themedItem(Q_NULLPTR)
    , attachedThemer(Q_NULLPTR)
    , theme(&UCTheme::defaultTheme())
    , themeType(Inherited)
{
}

// set the parent of the theme if the themeType is Custom
void UCThemingExtension::setParentTheme()
{
    if (themeType != Custom) {
        return;
    }
    QQuickItem *upperThemed = ascendantThemed(QQuickItemPrivate::get(themedItem)->parentItem);
    UCItemAttached *attached = static_cast<UCItemAttached*>(qmlAttachedPropertiesObject<UCItemAttached>(upperThemed));
    UCTheme *parentTheme = (attached && attached->m_extension) ? attached->m_extension->getTheme() : &UCTheme::defaultTheme();
    if (parentTheme != theme) {
        theme->setParentTheme(parentTheme);
    }
}

void UCThemingExtension::initTheming(QQuickItem *item)
{
    themedItem = item;
    attachedThemer = static_cast<UCItemAttached*>(qmlAttachedPropertiesObject<UCItemAttached>(themedItem));
    Q_ASSERT(attachedThemer);
    attachedThemer->m_extension = this;
    theme->attachItem(item, true);
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
        case Custom: {
            // emit theme's parentThemeChanged()
            Q_EMIT theme->parentThemeChanged();
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
    UCItemAttached *attached = static_cast<UCItemAttached*>(qmlAttachedPropertiesObject<UCItemAttached>(upperThemed));
    UCTheme *theme = (attached && attached->m_extension) ? attached->m_extension->getTheme() : &UCTheme::defaultTheme();
    setTheme(theme, Inherited);
}

// returns the closest themed ascendant
QQuickItem *UCThemingExtension::ascendantThemed(QQuickItem *item)
{
    while (item && !UCItemAttached::isThemed(item)) {
        // make sure it also has the theming attached
        qmlAttachedPropertiesObject<UCItemAttached>(item);
        item = item->parentItem();
    }
    return item;
}

#include "moc_ucthemingextension.cpp"
