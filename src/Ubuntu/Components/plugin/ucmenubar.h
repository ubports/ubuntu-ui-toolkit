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

#ifndef UCMENUBAR_H
#define UCMENUBAR_H

#include "ucmenu.h"

#include <QQmlParserStatus>

class UCMenuBarPrivate;
class QPlatformMenuBar;

class UCMenuBar : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(QQmlListProperty<UCMenu> menus READ menus NOTIFY menusChanged FINAL)
    Q_CLASSINFO("DefaultProperty", "menus")

public:
    explicit UCMenuBar(QObject *parent = 0);
    ~UCMenuBar();

    Q_INVOKABLE void appendMenu(UCMenu *menu);
    Q_INVOKABLE void insertMenu(int index, UCMenu *menu);
    Q_INVOKABLE void removeMenu(UCMenu *menu);

    QQmlListProperty<UCMenu> menus();

    QPlatformMenuBar *platformMenuBar() const;

    void classBegin() Q_DECL_OVERRIDE;
    void componentComplete() Q_DECL_OVERRIDE;

Q_SIGNALS:
    void menusChanged();

private:
    Q_DISABLE_COPY(UCMenuBar)
    Q_DECLARE_PRIVATE(UCMenuBar)
    QScopedPointer<UCMenuBarPrivate> d_ptr;
};

#endif // UCMENUBAR_H
