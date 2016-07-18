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

#ifndef MENUBAR_P_P_H
#define MENUBAR_P_P_H

#include "menubar_p.h"
#include <ubuntutoolkitglobal.h>

class QPlatformMenuBar;

UT_NAMESPACE_BEGIN

class PlatformMenuWrapper;

class MenuBarPrivate
{
    Q_DECLARE_PUBLIC(MenuBar)
public:
    MenuBarPrivate(MenuBar *qq);
    ~MenuBarPrivate();

    void insertMenu(int index, Menu *menu);
    void removeMenu(Menu *menu);

    static void menu_append(QQmlListProperty<Menu> *prop, Menu *o);
    static int menu_count(QQmlListProperty<Menu> *prop);
    static Menu *menu_at(QQmlListProperty<Menu> *prop, int index);
    static void menu_clear(QQmlListProperty<Menu> *prop);

    MenuBar* q_ptr;
    QPlatformMenuBar* m_platformBar;
    QVector<Menu*> m_menus;
    QHash<QObject*, PlatformMenuWrapper*> m_platformMenus;
};

class PlatformMenuWrapper : public QObject
{
    Q_OBJECT
public:
    PlatformMenuWrapper(Menu *target, MenuBar *bar);
    ~PlatformMenuWrapper();

    void insert(QPlatformMenu *before);
    void remove();

public Q_SLOTS:
    void updateVisible();
    void updateEnabled();
    void updateText();
    void updateIcon();

private:
    void syncPlatformMenu();

    QPointer<MenuBar> m_bar;
    QPointer<Menu> m_target;
    bool m_inserted;
};

UT_NAMESPACE_END

#endif // MENUBAR_P_P_H

