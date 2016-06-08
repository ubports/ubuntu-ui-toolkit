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

#include "ucmenubar.h"
#include "ucmenubar_p.h"

// Qt
#include <QQuickItem>
#include <QQuickWindow>
#include <private/qguiapplication_p.h>
#include <QtGui/qpa/qplatformtheme.h>
#include <QtGui/qpa/qplatformmenu.h>

UCMenuBarPrivate::UCMenuBarPrivate(UCMenuBar *qq)
    : q_ptr(qq)
{
    m_platformBar = QGuiApplicationPrivate::platformTheme()->createPlatformMenuBar();
}

UCMenuBarPrivate::~UCMenuBarPrivate()
{
    qDeleteAll(m_platformMenus);
    m_platformMenus.clear();

    delete m_platformBar;
    m_platformBar = Q_NULLPTR;
}

void UCMenuBarPrivate::insertMenu(int index, UCMenu* menu)
{
    Q_Q(UCMenuBar);
    UCMenu* prevMenu = m_menus.count() > index ? m_menus[index] : Q_NULLPTR;

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

void UCMenuBarPrivate::removeMenu(UCMenu *menu)
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

void UCMenuBarPrivate::menu_append(QQmlListProperty<UCMenu> *prop, UCMenu *o)
{
    UCMenuBarPrivate *q = static_cast<UCMenuBarPrivate *>(prop->data);
    // menubar is the menus parent
    o->setParent(prop->object);
    q->insertMenu(q->m_menus.count(), o);
}

int UCMenuBarPrivate::menu_count(QQmlListProperty<UCMenu> *prop)
{
    UCMenuBarPrivate *p = static_cast<UCMenuBarPrivate *>(prop->data);
    return p->m_menus.count();
}

UCMenu *UCMenuBarPrivate::menu_at(QQmlListProperty<UCMenu> *prop, int index)
{
    UCMenuBarPrivate *p = static_cast<UCMenuBarPrivate *>(prop->data);
    return p->m_menus.value(index);
}

void UCMenuBarPrivate::menu_clear(QQmlListProperty<UCMenu> *prop)
{
    UCMenuBarPrivate *p = static_cast<UCMenuBarPrivate *>(prop->data);
    p->m_menus.clear();
}

/*!
 * \qmltype MenuBar
 * \instantiates UCMenuBar
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
UCMenuBar::UCMenuBar(QObject *parent)
    : QObject(parent)
    , d_ptr(new UCMenuBarPrivate(this))
{
}

UCMenuBar::~UCMenuBar()
{
}

/*!
  * \qmlmethod void MenuBar::appendMenu(Menu menu)
  * Append a Menu to the MenuBar
 */
void UCMenuBar::appendMenu(UCMenu *menu)
{
    Q_D(UCMenuBar);
    insertMenu(d->m_menus.count(), menu);
}

/*!
  * \qmlmethod void MenuBar::insertMenu(int index, Menu menu)
  * Insert a Menu to the MenuBar at the specified position
 */
void UCMenuBar::insertMenu(int index, UCMenu *menu)
{
    Q_D(UCMenuBar);
    if (!menu) return;

    d->insertMenu(index, menu);
    Q_EMIT menusChanged();
}

/*!
  * \qmlmethod void MenuBar::removeMenu(Menu menu)
  * Remove a Menu from the MenuBar
 */
void UCMenuBar::removeMenu(UCMenu *menu)
{
    Q_D(UCMenuBar);
    if (!menu) return;

    d->removeMenu(menu);
    Q_EMIT menusChanged();
}
/*!
 * \qmlproperty list<Menu> MenuBar::menus
 * \default
 * List of Menus in this MenuBar.
 */
QQmlListProperty<UCMenu> UCMenuBar::menus()
{
    Q_D(UCMenuBar);
    return QQmlListProperty<UCMenu>(this, d,
                                     &UCMenuBarPrivate::menu_append,
                                     &UCMenuBarPrivate::menu_count,
                                     &UCMenuBarPrivate::menu_at,
                                    &UCMenuBarPrivate::menu_clear);
}

QPlatformMenuBar *UCMenuBar::platformMenuBar() const
{
    Q_D(const UCMenuBar);
    return d->m_platformBar;
}

void UCMenuBar::classBegin()
{
}

void UCMenuBar::componentComplete()
{
    Q_D(UCMenuBar);

    auto parentItem = qobject_cast<QQuickItem*>(parent());
    if (parentItem && d->m_platformBar) {
        d->m_platformBar->handleReparent(parentItem->window());
    }
}


PlatformMenuWrapper::PlatformMenuWrapper(UCMenu *target, UCMenuBar* bar)
    : QObject(bar)
    , m_bar(bar)
    , m_target(target)
    , m_inserted(false)
{
    connect(m_target, &UCMenu::visibleChanged, this, &PlatformMenuWrapper::updateVisible);
    connect(m_target, &UCMenu::textChanged, this, &PlatformMenuWrapper::updateText);
    connect(m_target, &UCMenu::enabledChanged, this, &PlatformMenuWrapper::updateEnabled);
    connect(m_target, &UCMenu::iconSourceChanged, this, &PlatformMenuWrapper::updateIcon);
    connect(m_target, &UCMenu::iconNameChanged, this, &PlatformMenuWrapper::updateIcon);

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
    qCInfo(ucMenu).nospace() << " PlatformMenuWrapper::insert(bar=" << m_bar
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
    qCInfo(ucMenu).nospace() << " PlatformMenuWrapper::remove(bar=" << m_bar
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

#include "moc_ucmenubar.cpp"
