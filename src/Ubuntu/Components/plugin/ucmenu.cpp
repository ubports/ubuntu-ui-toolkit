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
#include "ucmenugroup.h"

// Qt
#include <QtGui/qpa/qplatformtheme.h>
#include <QtGui/qpa/qplatformmenu.h>
#include <QQuickItem>
#include <private/qguiapplication_p.h>
#include <private/qquickitem_p.h>
#include <QPointer>
#include <functional>


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

// recursively get the all the object from the menu group which are not lists or groups.
QObjectList getActionsFromMenuGroup(UCMenuGroup* menuGroup) {

    QObjectList objectList;

    Q_FOREACH(QObject* data, menuGroup->list()) {

        if (auto actionList = qobject_cast<UCActionList*>(data)) {
            Q_FOREACH(UCAction* action, actionList->list()) {
                objectList << action;
            }
        } else if (auto subMenuGroup = qobject_cast<UCMenuGroup*>(data)) {
            objectList << getActionsFromMenuGroup(subMenuGroup);
        } else {
            objectList << data;
        }
    }
    return objectList;
}

// recursively get the first object from the menu group which is not a list or a group.
QObject* getFirstObject(UCMenuGroup* menuGroup) {

    Q_FOREACH(QObject* data, menuGroup->list()) {

        if (auto subMenuGroup = qobject_cast<UCMenuGroup*>(data)) {
            QObject* object = getFirstObject(subMenuGroup);
            if (object) { return object; }
        } else if (auto actionList = qobject_cast<UCActionList*>(data)) {
            return actionList->list().count() > 0 ? actionList->list()[0] : 0;
        } else {
            return data;
        }
    }
    return Q_NULLPTR;
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
    m_platformMenu = Q_NULLPTR;
}

void UCMenuPrivate::insertObject(int index, QObject *o)
{
    Q_Q(UCMenu);
    if (!o) return;
    qCInfo(ucMenu).nospace() << "UCMenu::insertObject(index="<< index << ", object=" << o << ")";

    if (!m_platformMenu) {
        QVector<QObject*>::iterator position = m_data.count() > index ? m_data.begin() + index : m_data.end();
        m_data.insert(position, o);
        return;
    }

    // If the menus contains lists or groups, we need to alter the insertion index to account for them.
    int actualIndex = 0;
    bool insertSeparator = false;
    for (int i = 0; i < index && i < m_data.count(); i++) {
        QObject* data = m_data[i];

        int dataObjectCount = 0;
        if (auto menuGroup = qobject_cast<UCMenuGroup*>(data)) {
            dataObjectCount = getActionsFromMenuGroup(menuGroup).count();
        } else if (auto list = qobject_cast<UCActionList*>(data)) {
            dataObjectCount = list->list().count();
        } else {
            dataObjectCount = 1;
        }
        actualIndex += dataObjectCount;
        // insert a separator before the new item if there are previous items.
        insertSeparator |= dataObjectCount > 0;

        // account for item separators if it's not the last data item.
        actualIndex += dataObjectCount > 0 && (i+1 < index && i+1 < m_data.count()) ? 1: 0;
    }

    // need to make sure the item after the insertion index has a separator
    if (index < m_data.count()) {
        QObject* data = m_data.at(index);
        QObject* objectToSeparate(Q_NULLPTR);

        if (auto menuGroup = qobject_cast<UCMenuGroup*>(data)) {
            objectToSeparate = getFirstObject(menuGroup);
        } else if (auto actionList = qobject_cast<UCActionList*>(data)) {
            objectToSeparate = actionList->list().count() > 0 ? actionList->list()[0] : 0;
        } else {
            objectToSeparate = data;
        }

        if (objectToSeparate && m_platformItems.contains(objectToSeparate)) {
            m_platformItems.value(objectToSeparate)->setSeparator();
        }
    }

    QVector<QObject*>::iterator position = m_data.count() > index ? m_data.begin() + index : m_data.end();
    m_data.insert(position, o);

    // if an object changes, we need to remove and re-add it.
    std::function<void()> refreshObject = [o, this]() {
        int index = m_data.indexOf(o);
        if (index >= 0)  {
            removeObject(o);
            insertObject(index, o);
        }
    };

    // Get All the menu item objects
    QObjectList objects;
    if (auto menuGroup = qobject_cast<UCMenuGroup*>(o)) {
        Q_FOREACH(QObject* menuGroupObject, getActionsFromMenuGroup(menuGroup)) {
            objects << menuGroupObject;
        }
        // connect to content changes
        QObject::connect(menuGroup, &UCMenuGroup::changed, q, refreshObject);
    } else if (auto actionList = qobject_cast<UCActionList*>(o)) {
        Q_FOREACH(UCAction* action, actionList->list()) {
            objects << action;
        }
        // connect to content changes
        QObject::connect(actionList, &UCActionList::added, q, refreshObject);
        QObject::connect(actionList, &UCActionList::removed, q, refreshObject);
    } else {
        objects << o;
    }

    Q_FOREACH(QObject* platformObject, objects) {
        // add to platform
        auto platformWrapper = new PlatformItemWrapper(platformObject, q);
        platformWrapper->insert(actualIndex++, insertSeparator);
        if (platformWrapper->hasSeparator()) { // we also inserted an separator, need to increment for next position.
            actualIndex++;
        }
        insertSeparator = false;
        m_platformItems[platformObject] = platformWrapper;
        // map the inserted item with the object which sources the platformItem info.
        if (!m_dataPlatformObjectMap.contains(o, platformObject)) {
            m_dataPlatformObjectMap.insertMulti(o, platformObject);
        }

        QObject::connect(platformObject, &QObject::destroyed, q, [o, this](QObject* platformObject) {
            m_dataPlatformObjectMap.remove(o, platformObject);

            if (m_platformItems.contains(platformObject)) {
                PlatformItemWrapper* platformItem = m_platformItems.take(platformObject);
                platformItem->remove();
                delete platformItem;
            }
        });
    }
}

void UCMenuPrivate::removeObject(QObject *o)
{
    Q_Q(UCMenu);
    m_data.removeOne(o);
    qCInfo(ucMenu).nospace() << "UCMenu::removeObject(" << o << ")";

    if (m_platformMenu) {
        if (auto menuGroup = qobject_cast<UCMenuGroup*>(o)) {
            // disconnect from content changes
            QObject::disconnect(menuGroup, &UCMenuGroup::changed, q, 0);
        }  else if (UCActionList* actionList = qobject_cast<UCActionList*>(o)) {
            // disconnect from content changes
            QObject::disconnect(actionList, &UCActionList::added, q, 0);
            QObject::disconnect(actionList, &UCActionList::removed, q, 0);
        }

        QList<QObject*> platformObjects = m_dataPlatformObjectMap.values(o);
        m_dataPlatformObjectMap.remove(o);

        Q_FOREACH(QObject* platformObject, platformObjects) {
            // remove from platform.
            if (m_platformItems.contains(platformObject)) {
                PlatformItemWrapper* platformItem = m_platformItems.take(platformObject);
                platformItem->remove();
                delete platformItem;
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
 * Currently supports Menu, Action, AcionList & MenuGroup objects.
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
 * Currently supports Menu, Action, AcionList & MenuGroup objects.
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
        d->m_platformMenu->showPopup(findWindowForObject(this), QRect(point, QSize()), Q_NULLPTR);
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


PlatformItemWrapper::PlatformItemWrapper(QObject *target, UCMenu* menu)
    : QObject(menu)
    , m_target(target)
    , m_menu(menu)
    , m_platformItem(menu->platformMenu() ? menu->platformMenu()->createMenuItem() : Q_NULLPTR)
    , m_platformItemSeparator(Q_NULLPTR)
    , m_inserted(false)
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

    }
    syncPlatformItem();
}

PlatformItemWrapper::~PlatformItemWrapper()
{
    if (m_inserted && m_menu && m_menu->platformMenu()) {
        m_menu->platformMenu()->removeMenuItem(m_platformItem);
        if (m_platformItemSeparator) {
            m_menu->platformMenu()->removeMenuItem(m_platformItemSeparator);
            delete m_platformItemSeparator;
        }
    }
    delete m_platformItem;
}

void PlatformItemWrapper::insert(int index, bool withSeparator)
{
    if (m_inserted) return;
    qCInfo(ucMenu).nospace() << " PlatformItemWrapper::insert(menu=" << m_menu
                                                        << ", index=" << index
                                                        << ", object=" << m_target << ")";

    auto platformMenu = m_menu->platformMenu();
    if (!platformMenu) return;
    if (!m_platformItem) return;

    QPlatformMenuItem* before = platformMenu->menuItemAt(index);
    platformMenu->insertMenuItem(m_platformItem, before);
    m_inserted = true;

    if (withSeparator) setSeparator();
}

void PlatformItemWrapper::remove()
{
    if (!m_inserted) return;
    qCInfo(ucMenu).nospace() << " PlatformItemWrapper::remove(menu=" << m_menu
                                                        << ", object=" << m_target << ")";

    auto platformMenu = m_menu->platformMenu();
    if (!platformMenu) return;
    if (!m_platformItem) return;

    platformMenu->removeMenuItem(m_platformItem);
    m_inserted = false;

    if (m_platformItemSeparator) {
        qCInfo(ucMenu).nospace() << " PlatformItemWrapper::removeSeparator(menu=" << m_menu
                                                            << ", object=" << m_target << ")";
        platformMenu->removeMenuItem(m_platformItemSeparator);
        delete m_platformItemSeparator;
        m_platformItemSeparator = Q_NULLPTR;
    }
}

void PlatformItemWrapper::setSeparator()
{
    // already created
    if (m_platformItemSeparator) return;
    // not inserted yet.
    if (!m_inserted) return;

    auto platformMenu = m_menu->platformMenu();
    if (!platformMenu) return;

    // insert separator before?
    m_platformItemSeparator = platformMenu->createMenuItem();
    if (m_platformItemSeparator) {
        qCInfo(ucMenu).nospace() << " PlatformItemWrapper::setSeparator(menu=" << m_menu
                                                            << ", object=" << m_target << ")";

        m_platformItemSeparator->setIsSeparator(true);
        platformMenu->insertMenuItem(m_platformItemSeparator, m_platformItem);
    }
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

#include "moc_ucmenu.cpp"
