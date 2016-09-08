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
 */

#include "menubar_p.h"
#include "menubar_p_p.h"

// Qt
#include <QQuickItem>
#include <QQuickWindow>
#include <QLoggingCategory>
#include <private/qguiapplication_p.h>
#include <QtGui/qpa/qplatformtheme.h>
#include <QtGui/qpa/qplatformmenu.h>

UT_NAMESPACE_BEGIN

MenuBarPrivate::MenuBarPrivate(MenuBar *qq)
    : q_ptr(qq)
{
    m_platformBar = QGuiApplicationPrivate::platformTheme()->createPlatformMenuBar();
}

MenuBarPrivate::~MenuBarPrivate()
{
    qDeleteAll(m_platformMenus);
    m_platformMenus.clear();

    delete m_platformBar;
    m_platformBar = Q_NULLPTR;
}

void MenuBarPrivate::insertMenu(int index, Menu* menu)
{
    Q_Q(MenuBar);
    Menu* prevMenu = m_menus.count() > index ? m_menus[index] : Q_NULLPTR;

    m_menus.insert(index, menu);

    // add to platform
    if (m_platformBar && menu->platformMenu()) {
        auto platformWrapper = new PlatformMenuWrapper(menu, q);
        platformWrapper->insert(prevMenu ? prevMenu->platformMenu() : Q_NULLPTR);
        m_platformMenus[menu] = platformWrapper;

        QObject::connect(menu, &QObject::destroyed, q, [this](QObject* object) {
            if (m_platformMenus.contains(object)) {
                PlatformMenuWrapper* wrapper = m_platformMenus.take(object);
                wrapper->remove();
                delete wrapper;
            }
        });
    }
}

void MenuBarPrivate::removeMenu(Menu *menu)
{
    m_menus.removeOne(menu);

    if (m_platformBar) {
        if (m_platformMenus.contains(menu)) {
            PlatformMenuWrapper* wrapper = m_platformMenus.take(menu);
            wrapper->remove();
            delete wrapper;
        }
    }
}

void MenuBarPrivate::menu_append(QQmlListProperty<Menu> *prop, Menu *o)
{
    MenuBarPrivate *q = static_cast<MenuBarPrivate *>(prop->data);
    // menubar is the menus parent
    o->setParent(prop->object);
    q->insertMenu(q->m_menus.count(), o);
}

int MenuBarPrivate::menu_count(QQmlListProperty<Menu> *prop)
{
    MenuBarPrivate *p = static_cast<MenuBarPrivate *>(prop->data);
    return p->m_menus.count();
}

Menu *MenuBarPrivate::menu_at(QQmlListProperty<Menu> *prop, int index)
{
    MenuBarPrivate *p = static_cast<MenuBarPrivate *>(prop->data);
    return p->m_menus.value(index);
}

void MenuBarPrivate::menu_clear(QQmlListProperty<Menu> *prop)
{
    MenuBarPrivate *p = static_cast<MenuBarPrivate *>(prop->data);
    p->m_menus.clear();
}

/*!
 * \qmltype MenuBar
 * \inqmlmodule Ubuntu.Components 1.3
 * \ingroup ubuntu
 * \brief MenuBar defines an application menu bar structure
 *
 * Example usage:
 * \qml
 * import QtQuick 2.4
 * import Ubuntu.Components 1.3
 * MainView {
 *     MenuBar {
 *         Menu {
 *             text: "_File"
 *
 *             MenuItem {
 *                 text: "_New"
 *                 shortcut: "Ctrl+N"
 *             }
 *
 *             MenuItem {
 *                 text: "_Open"
 *                 shortcut: "Ctrl+O"
 *             }
 *
 *             MenuSeparator {}
 *
 *             MenuItem {
 *                 action: exitAction
 *             }
 *         }
 *
 *         Menu {
 *             text: "_Edit"
 *
 *             MenuItem {
 *                 text: "_Undo"
 *                 iconSource: "image://theme/undo"
 *             }
 *         }
 *         Menu {
 *             text: "_Window"
 *
 *             MenuItem {
 *                 text: "Fullscreen"
 *                 checkable: true
 *                 checked: false
 *             }
 *         }
 *     }
 *     Action {
 *         id: boundAction
 *         text: "E_xit"
 *         onTriggered: {
 *             Qt.quit();
 *         }
 *     }
 * }
 * \endqml
 */
MenuBar::MenuBar(QObject *parent)
    : QObject(parent)
    , d_ptr(new MenuBarPrivate(this))
{
}

MenuBar::~MenuBar()
{
}

/*!
  * \qmlmethod void MenuBar::appendMenu(Menu menu)
  * Append a Menu to the MenuBar
 */
void MenuBar::appendMenu(Menu *menu)
{
    Q_D(MenuBar);
    insertMenu(d->m_menus.count(), menu);
}

/*!
  * \qmlmethod void MenuBar::insertMenu(int index, Menu menu)
  * Insert a Menu to the MenuBar at the specified position
 */
void MenuBar::insertMenu(int index, Menu *menu)
{
    Q_D(MenuBar);
    if (!menu) return;

    d->insertMenu(index, menu);
    Q_EMIT menusChanged();
}

/*!
  * \qmlmethod void MenuBar::removeMenu(Menu menu)
  * Remove a Menu from the MenuBar
 */
void MenuBar::removeMenu(Menu *menu)
{
    Q_D(MenuBar);
    if (!menu) return;

    d->removeMenu(menu);
    Q_EMIT menusChanged();
}
/*!
 * \qmlproperty list<Menu> MenuBar::menus
 * \default
 * List of Menus in this MenuBar.
 */
QQmlListProperty<Menu> MenuBar::menus()
{
    Q_D(MenuBar);
    return QQmlListProperty<Menu>(this, d,
                                     &MenuBarPrivate::menu_append,
                                     &MenuBarPrivate::menu_count,
                                     &MenuBarPrivate::menu_at,
                                    &MenuBarPrivate::menu_clear);
}

QPlatformMenuBar *MenuBar::platformMenuBar() const
{
    Q_D(const MenuBar);
    return d->m_platformBar;
}

void MenuBar::classBegin()
{
}

void MenuBar::componentComplete()
{
    Q_D(MenuBar);

    auto parentItem = qobject_cast<QQuickItem*>(parent());
    if (parentItem && d->m_platformBar) {
        d->m_platformBar->handleReparent(parentItem->window());
    }
}


PlatformMenuWrapper::PlatformMenuWrapper(Menu *target, MenuBar* bar)
    : QObject(bar)
    , m_bar(bar)
    , m_target(target)
    , m_inserted(false)
{
    connect(m_target, &Menu::visibleChanged, this, &PlatformMenuWrapper::updateVisible);
    connect(m_target, &Menu::textChanged, this, &PlatformMenuWrapper::updateText);
    connect(m_target, &Menu::enabledChanged, this, &PlatformMenuWrapper::updateEnabled);
    connect(m_target, &Menu::iconSourceChanged, this, &PlatformMenuWrapper::updateIcon);
    connect(m_target, &Menu::iconNameChanged, this, &PlatformMenuWrapper::updateIcon);

    syncPlatformMenu();
}

PlatformMenuWrapper::~PlatformMenuWrapper()
{
    if (m_inserted && m_bar && m_bar->platformMenuBar()) {
        if (m_target && m_target->platformMenu()) {
            m_bar->platformMenuBar()->removeMenu(m_target->platformMenu());
        }
    }
}

void PlatformMenuWrapper::insert(QPlatformMenu *before)
{
    if (m_inserted) return;
    qCDebug(ucMenu).nospace() << " PlatformMenuWrapper::insert(bar=" << m_bar
                                                        << ", before=" << before
                                                        << ", menu=" << m_target << ")";

    auto platformBar = m_bar->platformMenuBar();
    if (!platformBar) return;
    auto platformMenu = m_target->platformMenu();
    if (!platformMenu) return;

    platformBar->insertMenu(platformMenu, before);
    m_inserted = true;
}

void PlatformMenuWrapper::remove()
{
    if (!m_inserted) return;
    qCDebug(ucMenu).nospace() << " PlatformMenuWrapper::remove(bar=" << m_bar
                                                        << ", menu=" << m_target << ")";

    auto platformBar = m_bar->platformMenuBar();
    if (!platformBar) return;
    auto platformMenu = m_target->platformMenu();
    if (!platformMenu) return;

    platformBar->removeMenu(platformMenu);
    m_inserted = false;
}

void PlatformMenuWrapper::updateVisible()
{
    if (m_target->platformMenu()) m_target->platformMenu()->setVisible(m_target->visible());
}

void PlatformMenuWrapper::updateEnabled()
{
    if (m_target->platformMenu()) m_target->platformMenu()->setEnabled(m_target->isEnabled());
}

void PlatformMenuWrapper::updateText()
{
    if (m_target->platformMenu()) m_target->platformMenu()->setText(m_target->text());
}

void PlatformMenuWrapper::updateIcon()
{
    QIcon icon;
    if (!m_target->iconSource().isEmpty()) {
        icon = QIcon(m_target->iconSource().path());
    } else if (!m_target->iconName().isEmpty()) {
        icon = QIcon::fromTheme(m_target->iconName());
    }
    if (m_target->platformMenu()) m_target->platformMenu()->setIcon(icon);
}

void PlatformMenuWrapper::syncPlatformMenu()
{
    updateVisible();
    updateEnabled();
    updateText();
    updateIcon();
}

UT_NAMESPACE_END

#include "moc_menubar_p.cpp"
