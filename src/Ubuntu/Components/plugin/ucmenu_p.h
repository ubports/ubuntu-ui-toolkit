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

#ifndef UCMENU_P
#define UCMENU_P

#include <private/qquickitemchangelistener_p.h>
#include "ucmenu.h"

class QObject;
class UCAction;
class QQmlComponent;
class UCMenu;
class PlatformItemWrapper;

class UCMenuPrivate
{
    Q_DECLARE_PUBLIC(UCMenu)
public:
    UCMenuPrivate(UCMenu *qq);
    virtual ~UCMenuPrivate();

    void insertObject(int index, QObject *obj);
    void removeObject(QObject *obj);

    void _q_updateEnabled();
    void _q_updateText();
    void _q_updateIcon();
    void _q_updateVisible();

    static void data_append(QQmlListProperty<QObject> *prop, QObject *o);
    static int data_count(QQmlListProperty<QObject> *prop);
    static QObject *data_at(QQmlListProperty<QObject> *prop, int index);
    static void data_clear(QQmlListProperty<QObject> *prop);

    UCMenu* q_ptr;
    QPlatformMenu* m_platformMenu;
    UCAction* m_action;

    QHash<QObject*, PlatformItemWrapper*> m_platformItems;
    QMultiHash<QObject*, QObject*> m_dataPlatformObjectMap;
    QVector<QObject*> m_data;
};

class PlatformItemWrapper : public QObject
{
    Q_OBJECT
public:
    PlatformItemWrapper(QObject *target, UCMenu* menu);
    ~PlatformItemWrapper();

    void insert(int index, bool withSeparator);
    void remove();
    void setSeparator();

    bool hasSeparator() const { return m_platformItemSeparator != Q_NULLPTR; }

public Q_SLOTS:
    void updateVisible();
    void updateEnabled();
    void updateText();
    void updateIcon();
    void updateShortcut();
    void updateCheck();

private:
    void syncPlatformItem();

    QObject* m_target;
    QPointer<UCMenu> m_menu;
    QPlatformMenuItem* m_platformItem;
    QPlatformMenuItem* m_platformItemSeparator;
    bool m_inserted;
};

#endif // UCMENU_P
