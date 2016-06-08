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
 */

#include "ucmenu.h"
#include "ucmenu_p.h"
#include "ucmenubar.h"
#include "ucaction.h"
#include "ucactionlist.h"

// Qt
#include <QtGui/qpa/qplatformtheme.h>
#include <QtGui/qpa/qplatformmenu.h>
#include <QQuickItem>
#include <private/qguiapplication_p.h>
#include <private/qquickitem_p.h>
#include <QPointer>


Q_LOGGING_CATEGORY(ucMenu, "ubuntu.components.Menu", QtMsgType::QtWarningMsg)

namespace {

QWindow* findWindowForObject(QObject* object)
{
    QObject* window = object;
    while (window && !window->isWindowType()) {
        window = window->parent();
        if (QQuickItem* item = qobject_cast<QQuickItem*>(window)) {
            window = item->window();
        }
    }
    return qobject_cast<QWindow*>(window);
}

}

UCMenuPrivate::UCMenuPrivate(UCMenu *qq)
    : q_ptr(qq)
    , m_platformMenu(QGuiApplicationPrivate::platformTheme()->createPlatformMenu())
{
}

UCMenuPrivate::~UCMenuPrivate()
{
    qDeleteAll(m_platformItems);
    m_platformItems.clear();

    delete m_platformMenu;
    m_platformMenu = nullptr;
}

void UCMenuPrivate::insertObject(int index, QObject *o)
{
    Q_Q(UCMenu);
    if (!o) return;
    qCInfo(ucMenu).nospace() << "UCMenu::insertObject(index="<< index << ", object=" << o << ")";

    // If the menus contains lists, we need to alter the insertion index to account for them.
    int actualIndex = 0;
    for (int i = 0; i < index && i < m_data.count(); i++) {
        QObject* data = m_data[i];
        auto list = qobject_cast<UCActionList*>(data);
        actualIndex += list ? list->list().count() : 1;

        // menu group adds a separator item.
        if (qobject_cast<UCMenuGroup*>(data)) {
            actualIndex++;
        }
    }

    QVector<QObject*>::iterator position = m_data.count() > index ? m_data.begin() + index : m_data.end();
    m_data.insert(position, o);

    QObjectList objects;
    if (UCActionList* actionList = qobject_cast<UCActionList*>(o)) {
        Q_FOREACH(UCAction* action, actionList->list()) {
            qCInfo(ucMenu).nospace() << " UCMenu::insertObject(actionList=" << actionList << ", action=" << action << ")";
            objects << action;
        }
        // menu group adds a separator item; so add self as well.
        if (qobject_cast<UCMenuGroup*>(actionList)) {
            objects << o;
        }
    } else {
        objects << o;
    }

    Q_FOREACH(QObject* object, objects) {

        UCMenuAttached *attached = qobject_cast<UCMenuAttached*>(qmlAttachedPropertiesObject<UCMenuAttached>(object));
        if (attached) {
            attached->setParentObject(q);
        }

        // add to platform
        if (m_platformMenu) {
            auto platformWrapper = new PlatformItemWrapper(object, q);
            platformWrapper->insert(actualIndex++);
            m_platformItems[object] = platformWrapper;

            QObject::connect(object, &QObject::destroyed, q, [this](QObject* object) {
                if (m_platformItems.contains(object)) {
                    m_platformItems[object]->remove();
                    delete m_platformItems.take(object);
                }
            });
        }
    }
}

void UCMenuPrivate::removeObject(QObject *o)
{
    Q_Q(UCMenu);
    m_data.removeOne(o);
    qCInfo(ucMenu).nospace() << "UCMenu::insertObject(" << o << ")";

    if (m_platformMenu) {
        QObjectList objects;
        if (UCActionList* actionList = qobject_cast<UCActionList*>(o)) {
            Q_FOREACH(UCAction* action, actionList->list()) {
                objects << action;
            }
            if (qobject_cast<UCMenuGroup*>(actionList)) {
                objects << o;
            }
        } else {
            objects << o;
        }

        Q_FOREACH(QObject* object, objects) {
            // remove from platform.
            if (m_platformItems.contains(object)) {
                m_platformItems[object]->remove();
                delete m_platformItems.take(object);
            }
        }
    }
}

void UCMenuPrivate::_q_updateEnabled()
{
    Q_Q(UCMenu);

    bool isEnabled = q->isEnabled();
    if (m_platformMenu) { m_platformMenu->setEnabled(isEnabled); }
}

void UCMenuPrivate::_q_updateText()
{
    Q_Q(UCMenu);

    QString text = q->text();
    if (m_platformMenu) { m_platformMenu->setText(text); }
}

void UCMenuPrivate::_q_updateIcon()
{
    Q_Q(UCMenu);

    QIcon icon;
    if (!q->iconSource().isEmpty()) {
        icon = QIcon(q->iconSource().path());
    } else if (!q->iconName().isEmpty()) {
        icon = QIcon::fromTheme(q->iconName());
    }

    if (m_platformMenu) { m_platformMenu->setIcon(icon); }
}

void UCMenuPrivate::_q_updateVisible()
{
    Q_Q(UCMenu);

    bool visible = q->visible();
    if (m_platformMenu) { m_platformMenu->setVisible(visible); }
}

void UCMenuPrivate::data_append(QQmlListProperty<QObject> *prop, QObject *o)
{
    UCMenu *q = qobject_cast<UCMenu *>(prop->object);
    q->appendObject(o);
}

int UCMenuPrivate::data_count(QQmlListProperty<QObject> *prop)
{
    UCMenuPrivate *p = static_cast<UCMenuPrivate *>(prop->data);
    return p->m_data.count();
}

QObject *UCMenuPrivate::data_at(QQmlListProperty<QObject> *prop, int index)
{
    UCMenuPrivate *p = static_cast<UCMenuPrivate *>(prop->data);
    return p->m_data.value(index);
}

void UCMenuPrivate::data_clear(QQmlListProperty<QObject> *prop)
{
    UCMenuPrivate *p = static_cast<UCMenuPrivate *>(prop->data);
    p->m_data.clear();
}

/*!
 * \qmltype Menu
 * \instantiates UCMenu
 * \inqmlmodule Ubuntu.Components
 * \ingroup ubuntu
 * \brief Menu defines a context menu or submenu structure of a MenuBar
 *
 * Example usage:
 * \qml
 * import QtQuick 2.4
 * import Ubuntu.Components 1.3
 * Menu {
 *     text: "&File"
 *
 *     MenuGroup {
 *         Action {
 *             text: "&New"
 *             shortcut: "Ctrl+N"
 *         }
 *
 *         Action {
 *             text: "&Open"
 *             shortcut: "Ctrl+O"
 *         }
 *     }
 *
 *     Menu {
 *         text: "Recent Files"
 *
 *         ActionList {
 *             Action { text: "1.txt" }
 *             Action { text: "2.txt" }
 *             Action { text: "3.txt" }
 *         }
 *     }
 *
 *     Action {
 *         action: Action {
 *             text: "E&xit"
 *             shortcut: "Ctrl+X"
 *         }
 *     }
 * }
 * \endqml
 */
UCMenu::UCMenu(QObject *parent)
    : UCAction(parent)
    , d_ptr(new UCMenuPrivate(this))
{
    Q_D(UCMenu);

    connect(this, SIGNAL(enabledChanged()), this, SLOT(_q_updateEnabled()));
    connect(this, SIGNAL(textChanged()), this, SLOT(_q_updateText()));
    connect(this, SIGNAL(iconNameChanged()), this, SLOT(_q_updateIcon()));
    connect(this, SIGNAL(iconSourceChanged()), this, SLOT(_q_updateIcon()));
    connect(this, SIGNAL(visibleChanged()), this, SLOT(_q_updateVisible()));
}

UCMenu::~UCMenu()
{
}

/*!
 * \qmlproperty list<Object> Menu::data
 * \default
 * List of objects representing menu items within the menu.
 *
 * Currently supports Menu, MenuItem, MenuSeparator & Item objects.
 * \note Item object which do not support platformItem will not be exported for native menus.
 */
QQmlListProperty<QObject> UCMenu::data()
{
    Q_D(UCMenu);
    return QQmlListProperty<QObject>(this, d,
                                     &UCMenuPrivate::data_append,
                                     &UCMenuPrivate::data_count,
                                     &UCMenuPrivate::data_at,
                                     &UCMenuPrivate::data_clear);
}

/*!
 * \qmlmethod Menu::appendObject(object o)
 * Add a object tto the menu
 */
void UCMenu::appendObject(QObject *o)
{
    Q_D(UCMenu);

    insertObject(d->m_data.count(), o);
}

/*!
 * \qmlmethod Menu::insertObject(int index, object o)
 * Inserts an item at the index in the menu.
 *
 * Currently supports Menu, MenuItem, MenuSeparator & Item objects.
 * \note Item object which do not support platformItem will not be exported for native menus.
 */
void UCMenu::insertObject(int index, QObject *o)
{
    Q_D(UCMenu);
    d->insertObject(index, o);
}

/*!
 * \qmlmethod Menu::removeObject(object o)
 * Removes the item from the menu.
 */
void UCMenu::removeObject(QObject *o)
{
    Q_D(UCMenu);
    qCInfo(ucMenu) << "UCMenu::removeObject" << o;

    d->removeObject(o);
}

QPlatformMenu* UCMenu::platformMenu() const
{
    Q_D(const UCMenu);
    return d->m_platformMenu;
}

/*!
 * \qmlmethod Menu::show(point point)
 * Show the menu popup at the given point
 */
void UCMenu::show(const QPoint &point)
{
    Q_D(UCMenu);
    qCInfo(ucMenu, "UCMenu::popup(%s, point(%d,%d))", qPrintable(text()), point.x(), point.y());

    if (d->m_platformMenu) {
        d->m_platformMenu->showPopup(findWindowForObject(this), QRect(point, QSize()), nullptr);
    }
}

/*!
 * \qmlmethod Menu::dismiss()
 * Dismiss and destroy the menu popup.
 */
void UCMenu::dismiss()
{
    Q_D(UCMenu);
    qCInfo(ucMenu, "UCMenu::dismiss(%s)", qPrintable(text()));

    if (d->m_platformMenu) {
        d->m_platformMenu->dismiss();
    }
}

/*!
 * \qmltype Menus
 * \instantiates UCMenuAttached
 * \inqmlmodule Ubuntu.Components 1.3
 * \ingroup ubuntu
 * \since Ubuntu.Components 1.3
 * \brief A set of properties attached to the Menu.
 *
 */
UCMenuAttached::UCMenuAttached(QObject *parent)
    : QObject(parent)
    , m_parentObject(nullptr)
{
}

UCMenuAttached *UCMenuAttached::qmlAttachedProperties(QObject *o)
{
    return new UCMenuAttached(o);
}

void UCMenuAttached::setParentObject(QObject *o)
{
    if (m_parentObject != o) {
        UCMenu* oldMenu = parentMenu();
        UCMenuBar* oldBar = menuBar();

        m_parentObject = o;

        if (oldMenu != parentMenu()) Q_EMIT parentMenuChanged();
        if (oldBar != menuBar()) Q_EMIT menuBarChanged();
    }
}

/*!
 * \qmlattachedproperty bool Menus::parentMenu
 * The property returns the parent Menu of the object, or null if there is none.
 */
UCMenu *UCMenuAttached::parentMenu() const
{
    return qobject_cast<UCMenu*>(m_parentObject);
}

/*!
 * \qmlattachedproperty bool Menus::menuBar
 * The property returns the parent MenuBar of the object, or null if there is none.
 */
UCMenuBar *UCMenuAttached::menuBar() const
{
    return qobject_cast<UCMenuBar*>(m_parentObject);
}


PlatformItemWrapper::PlatformItemWrapper(QObject *target, UCMenu* menu)
    : QObject(menu)
    , m_target(target)
    , m_menu(menu)
    , m_platformItem(menu->platformMenu() ? menu->platformMenu()->createMenuItem() : Q_NULLPTR)
{
    if (UCMenu* menu = qobject_cast<UCMenu*>(m_target)) {
        if (m_platformItem) {
            m_platformItem->setMenu(menu->platformMenu());
        }

        connect(menu, &UCMenu::visibleChanged, this, &PlatformItemWrapper::updateVisible);
        connect(menu, &UCMenu::textChanged, this, &PlatformItemWrapper::updateText);
        connect(menu, &UCMenu::enabledChanged, this, &PlatformItemWrapper::updateEnabled);
        connect(menu, &UCMenu::iconSourceChanged, this, &PlatformItemWrapper::updateIcon);
        connect(menu, &UCMenu::iconNameChanged, this, &PlatformItemWrapper::updateIcon);

    } else if (UCAction* action = qobject_cast<UCAction*>(m_target)) {

        connect(action, &UCAction::visibleChanged, this, &PlatformItemWrapper::updateVisible);
        connect(action, &UCAction::textChanged, this, &PlatformItemWrapper::updateText);
        connect(action, &UCAction::enabledChanged, this, &PlatformItemWrapper::updateEnabled);
        connect(action, &UCAction::iconSourceChanged, this, &PlatformItemWrapper::updateIcon);
        connect(action, &UCAction::iconNameChanged, this, &PlatformItemWrapper::updateIcon);
        connect(action, &UCAction::shortcutChanged, this, &PlatformItemWrapper::updateShortcut);
        connect(action, &UCAction::parameterTypeChanged, this, &PlatformItemWrapper::updateCheck);
        connect(action, &UCAction::stateChanged, this, &PlatformItemWrapper::updateCheck);

        if (m_platformItem) {
            connect(m_platformItem, SIGNAL(activated()), action, SLOT(trigger()));
        }

    } else if (qobject_cast<UCActionList*>(m_target)) {
        if (m_platformItem) {
            m_platformItem->setIsSeparator(true);
        }
    }

    syncPlatformItem();
}

PlatformItemWrapper::~PlatformItemWrapper()
{
    delete m_platformItem;
}

void PlatformItemWrapper::insert(int index)
{
    auto platformMenu = m_menu->platformMenu();
    if (!platformMenu) return;
    if (!m_platformItem) return;

    QPlatformMenuItem* before = platformMenu->menuItemAt(index);
    platformMenu->insertMenuItem(m_platformItem, before);
}

void PlatformItemWrapper::remove()
{
    auto platformMenu = m_menu->platformMenu();
    if (!platformMenu) return;
    if (!m_platformItem) return;

    platformMenu->removeMenuItem(m_platformItem);
}

void PlatformItemWrapper::updateVisible()
{
    if (!m_platformItem) return;

    if (UCMenu* menu = qobject_cast<UCMenu*>(m_target)) {
        m_platformItem->setVisible(menu->visible());
        if (menu->platformMenu()) menu->platformMenu()->setVisible(menu->visible());
    } else if (UCAction* action = qobject_cast<UCAction*>(m_target)) {
        m_platformItem->setVisible(action->visible());
    }
}

void PlatformItemWrapper::updateEnabled()
{
    if (!m_platformItem) return;

    if (UCMenu* menu = qobject_cast<UCMenu*>(m_target)) {
        m_platformItem->setEnabled(menu->isEnabled());
        if (menu->platformMenu()) menu->platformMenu()->setEnabled(menu->isEnabled());
    } else if (UCAction* action = qobject_cast<UCAction*>(m_target)) {
        m_platformItem->setText(action->text());
    }
}

void PlatformItemWrapper::updateText()
{
    if (!m_platformItem) return;

    if (UCMenu* menu = qobject_cast<UCMenu*>(m_target)) {
        m_platformItem->setText(menu->text());
        if (menu->platformMenu()) menu->platformMenu()->setText(menu->text());
    } else if (UCAction* action = qobject_cast<UCAction*>(m_target)) {
        m_platformItem->setText(action->text());
    }
}

void PlatformItemWrapper::updateIcon()
{
    if (!m_platformItem) return;

    QIcon icon;
    if (UCMenu* menu = qobject_cast<UCMenu*>(m_target)) {

        if (!menu->iconSource().isEmpty()) {
            icon = QIcon(menu->iconSource().path());
        } else if (!menu->iconName().isEmpty()) {
            icon = QIcon::fromTheme(menu->iconName());
        }
        if (menu->platformMenu()) menu->platformMenu()->setIcon(icon);

    } else if (UCAction* action = qobject_cast<UCAction*>(m_target)) {

        if (!action->iconSource().isEmpty()) {
            icon = QIcon(action->iconSource().path());
        } else if (!action->iconName().isEmpty()) {
            icon = QIcon::fromTheme(action->iconName());
        }
    }
    m_platformItem->setIcon(icon);
}


inline QKeySequence sequenceFromVariant(const QVariant& variant)
{
    if (variant.type() == QVariant::Int) {
        return static_cast<QKeySequence::StandardKey>(variant.toInt());
    }
    if (variant.type() == QVariant::String) {
        return QKeySequence::fromString(variant.toString());
    }
    return QKeySequence();
}

void PlatformItemWrapper::updateShortcut()
{
    if (!m_platformItem) return;

    if (UCAction* action = qobject_cast<UCAction*>(m_target)) {
        m_platformItem->setShortcut(sequenceFromVariant(action->shortcut()));
    }
}

void PlatformItemWrapper::updateCheck()
{
    if (!m_platformItem) return;

    if (UCAction* action = qobject_cast<UCAction*>(m_target)) {
        bool checkable = action->parameterType() == UCAction::Bool;
        m_platformItem->setCheckable(checkable);
        m_platformItem->setChecked(checkable && action->state().toBool());
    }
}

void PlatformItemWrapper::syncPlatformItem()
{
    updateVisible();
    updateEnabled();
    updateText();
    updateIcon();
    updateShortcut();
    updateCheck();

    if (m_menu->platformMenu() && m_platformItem) {
        m_menu->platformMenu()->syncMenuItem(m_platformItem);
    }
}


/*!
 * \qmltype MenuGroup
 * \inqmlmodule Ubuntu.Components
 * \ingroup ubuntu
 * \brief List of \l Action items for a menu which adds a separator between logical groups of menus.
 *
 * Example usage:
 * \qml
 * import QtQuick 2.4
 * import Ubuntu.Components 1.3
 * Menu {
 *     text: "Edit"
 *
 *     MenuGroup {
 *         Action { text: "Undo" }
 *         Action { text: "Redo" }
 *     }
 *
 *     MenuGroup {
 *         Action { text: "Cut" }
 *         Action { text: "Copy" }
 *         Action { text: "Paste" }
 *     }
 *
 *     MenuGroup {
 *         Action { text: "Select All" }
 *     }
 * }
 * \endqml
 */
UCMenuGroup::UCMenuGroup(QObject *parent)
    : UCActionList(parent)
{
}

#include "moc_ucmenu.cpp"
