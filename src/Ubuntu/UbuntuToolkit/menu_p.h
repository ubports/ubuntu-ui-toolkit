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

#ifndef MENU_P_H
#define MENU_P_H

#include <QQmlListProperty>
#include <QLoggingCategory>
#include <QPointer>
#include <ubuntutoolkitglobal.h>

#include "ucaction_p.h"

Q_DECLARE_LOGGING_CATEGORY(ucMenu);

class QPlatformMenu;
class QPlatformMenuItem;
class QQuickItem;

UT_NAMESPACE_BEGIN

class MenuPrivate;
class MenuBar;
class UCAction;
class UBUNTUTOOLKIT_EXPORT Menu : public UCAction
{
    Q_OBJECT

    Q_PROPERTY(QQmlListProperty<QObject> data READ data FINAL)
    Q_CLASSINFO("DefaultProperty", "data")

public:
    explicit Menu(QObject *parent = 0);
    ~Menu();

    QQmlListProperty<QObject> data();

    Q_INVOKABLE void appendObject(QObject* obj);
    Q_INVOKABLE void insertObject(int index, QObject* obj);
    Q_INVOKABLE void removeObject(QObject* obj);

    QPlatformMenu *platformMenu() const;

public Q_SLOTS:
    void show(const QPoint& pt);
    void dismiss();

private:
    Q_DISABLE_COPY(Menu)
    Q_DECLARE_PRIVATE(Menu)
    QScopedPointer<MenuPrivate> d_ptr;

    Q_PRIVATE_SLOT(d_func(), void _q_updateEnabled())
    Q_PRIVATE_SLOT(d_func(), void _q_updateText())
    Q_PRIVATE_SLOT(d_func(), void _q_updateIcon())
    Q_PRIVATE_SLOT(d_func(), void _q_updateVisible())
};

UT_NAMESPACE_END

#endif // MENU_P_H
