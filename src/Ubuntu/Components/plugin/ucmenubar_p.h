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

#ifndef UCMENUBAR_P
#define UCMENUBAR_P

#include "ucmenubar.h"

class QPlatformMenuBar;

class UCMenuBarPrivate : public QQuickItemChangeListener
{
    Q_DECLARE_PUBLIC(UCMenuBar)
public:
    UCMenuBarPrivate(UCMenuBar *qq);

    void insertMenu(int index, UCMenu *menu);

    static void menu_append(QQmlListProperty<UCMenu> *prop, UCMenu *o);
    static int menu_count(QQmlListProperty<UCMenu> *prop);
    static UCMenu *menu_at(QQmlListProperty<UCMenu> *prop, int index);
    static void menu_clear(QQmlListProperty<UCMenu> *prop);

    UCMenuBar* q_ptr;
    QPlatformMenuBar* m_platformBar;
    QVector<UCMenu*> m_menus;
};

class PlatformMenuWrapper : public QObject
{
    Q_OBJECT
public:
    PlatformMenuWrapper(UCMenu *target, UCMenuBar *bar);

    void insert(int index);
    void remove();

public Q_SLOTS:
    void updateVisible();
    void updateEnabled();
    void updateText();
    void updateIcon();
    void updateShortcut();

private:
    void syncPlatformItem();

    UCMenuBar* m_bar;
    UCMenu* m_target;
    QPlatformMenuItem* m_platformItem;
};

#endif // UCMENUBAR_P

