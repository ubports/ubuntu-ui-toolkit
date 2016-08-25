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

#ifndef MENUBAR_P_H
#define MENUBAR_P_H

#include "menu_p.h"
#include <ubuntutoolkitglobal.h>

#include <QQmlParserStatus>

class QPlatformMenuBar;

UT_NAMESPACE_BEGIN

class MenuBarPrivate;
class UBUNTUTOOLKIT_EXPORT MenuBar : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(QQmlListProperty<UT_PREPEND_NAMESPACE(Menu)> menus READ menus NOTIFY menusChanged FINAL)
    Q_CLASSINFO("DefaultProperty", "menus")

public:
    explicit MenuBar(QObject *parent = 0);
    ~MenuBar();

    Q_INVOKABLE void appendMenu(Menu *menu);
    Q_INVOKABLE void insertMenu(int index, Menu *menu);
    Q_INVOKABLE void removeMenu(Menu *menu);

    QQmlListProperty<Menu> menus();

    QPlatformMenuBar *platformMenuBar() const;

    void classBegin() Q_DECL_OVERRIDE;
    void componentComplete() Q_DECL_OVERRIDE;

Q_SIGNALS:
    void menusChanged();

private:
    Q_DISABLE_COPY(MenuBar)
    Q_DECLARE_PRIVATE(MenuBar)
    QScopedPointer<MenuBarPrivate> d_ptr;
};

UT_NAMESPACE_END

#endif // MENUBAR_P_H
