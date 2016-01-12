/*
 * Copyright 2016 Canonical Ltd.
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
 * The UCThemingExtension interface provides theme handling on Items extending an existing
 * QQuickItem derivate class. Items subject fo theming should derive from this class
 * and implement the pure virtual methods.
 * The item can expose the theme property and use the getters defined by the
 * class.
 * In case the item exposes the theme property, it can use the getters from the
 * extension and declare the themeChanged signal.
 */

void notifyThemeChange(QQuickItem *item, UCTheme *oldTheme, UCTheme *newTheme)
{
    Q_FOREACH(QQuickItem *child, item->childItems()) {
        UCThemingExtension *extension = qobject_cast<UCThemingExtension*>(child);
        if (extension) {
            extension->itemThemeChanged(oldTheme, newTheme);
        }
        // StyledItem will handle the broadcast itself depending on whether the theme change was appropriate or not
        // and will complete the ascendantStyled/theme itself
        if (!extension) {
            notifyThemeChange(child, oldTheme, newTheme);
        }
    }
}

void notifyThemeReloaded(QQuickItem *item, UCTheme *theme)
{
    Q_FOREACH(QQuickItem *child, item->childItems()) {
        UCThemingExtension *extension = qobject_cast<UCThemingExtension*>(child);
        if (extension) {
            extension->itemThemeReloaded(theme);
        }
        // StyledItem will handle the broadcast itself depending on whether the theme change was appropriate or not
        // and will complete the ascendantStyled/theme itself
        if (!extension) {
            notifyThemeReloaded(child, theme);
        }
    }
}

/*************************************************************************
 * Attached to every Item in the system
 */
static uint xdata = QObject::registerUserData();
class UCItemAttached : public QObjectUserData, public QQuickItemChangeListener
{
public:
    explicit UCItemAttached(QQuickItem *owner = 0);
    ~UCItemAttached();

    QQuickItem *m_item;
    QQuickItem *m_prevParent;

    void itemParentChanged(QQuickItem *item, QQuickItem *newParent);

private:

    friend class UCThemingExtension;
};

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

    // when we set a parent, the two items must be under the same engine
    if (newParent && qmlEngine(m_item) != qmlEngine(newParent)) {
        return;
    }

    if (!qmlEngine(m_item)) {
        // the item is about to be deleted, parent change occurs as the parent is removed
        Q_ASSERT(!newParent);
        return;
    }

    // make sure we have these handlers attached to each intermediate item
    QQuickItem *oldThemedAscendant = UCThemingExtension::ascendantThemed(m_prevParent);
    QQuickItem *newThemedAscendant = UCThemingExtension::ascendantThemed(newParent);
    UCThemingExtension *oldExtension = qobject_cast<UCThemingExtension*>(oldThemedAscendant);
    UCThemingExtension *newExtension = qobject_cast<UCThemingExtension*>(newThemedAscendant);
    UCTheme *oldTheme = oldExtension ? oldExtension->getTheme() : UCTheme::defaultTheme(qmlEngine(m_item));
    UCTheme *newTheme = newExtension ? newExtension->getTheme() : UCTheme::defaultTheme(qmlEngine(m_item));

    // neither of the themes can be null!
    Q_ASSERT(oldTheme);
    Q_ASSERT(newTheme);

    if (oldTheme != newTheme) {
        UCThemingExtension *extension = qobject_cast<UCThemingExtension*>(m_item);
        // send the event to m_item first
        if (extension) {
            // only items with extensions should get this event
            extension->itemThemeChanged(oldTheme, newTheme);
        }
        // then broadcast to children, but only if the item is not a styled one
        if (!extension) {
            notifyThemeChange(m_item, oldTheme, newTheme);
        }
    }
    m_prevParent = newParent;
}

/*************************************************************************
 *
 */
UCThemingExtension::UCThemingExtension(QQuickItem *extendedItem)
    : theme(Q_NULLPTR)
    , themedItem(extendedItem)
    , themeType(Inherited)
{
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
    UCTheme *parentTheme = extension ? extension->getTheme() : UCTheme::defaultTheme(qmlEngine(themedItem));
    if (parentTheme != theme) {
        theme->setParentTheme(parentTheme);
    }
}

void UCThemingExtension::itemThemeChanged(UCTheme*, UCTheme *newTheme)
{
    switch (themeType) {
    case Inherited: {
        setTheme(newTheme, Inherited);
        return;
    }
    case Custom: {
        // set the theme's parent
        theme->setParentTheme(newTheme);
        return;
    }
    default: break;
    }
}

void UCThemingExtension::itemThemeReloaded(UCTheme *theme)
{
    switch (themeType) {
    case Inherited: {
        preThemeChanged();
        postThemeChanged();
        return;
    }
    case Custom: {
        if (theme == this->theme) {
            preThemeChanged();
            postThemeChanged();
            // forward to children
            notifyThemeReloaded(themedItem, theme);
        } else {
            // emit theme's parentThemeChanged()
            Q_EMIT this->theme->parentThemeChanged();
        }
        return;
    }
    default: break;
    }
}

UCTheme *UCThemingExtension::getTheme()
{
    if (!theme) {
        theme = UCTheme::defaultTheme(qmlEngine(themedItem));
        if (!theme) {
            QString msg = QStringLiteral("The item %1 was created without a valid QML Engine. Styling will not be possible.")
                    .arg(themedItem->metaObject()->className());
            qCritical().noquote() << msg;
            return Q_NULLPTR;
        }
        theme->attachItem(themedItem, true);
    }
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
    notifyThemeChange(themedItem, oldTheme, theme);
}

void UCThemingExtension::resetTheme()
{
    QQuickItem *upperThemed = ascendantThemed(QQuickItemPrivate::get(themedItem)->parentItem);
    UCThemingExtension *extension = qobject_cast<UCThemingExtension*>(upperThemed);
    UCTheme *theme = extension ? extension->getTheme() : UCTheme::defaultTheme(qmlEngine(themedItem));
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
