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

#ifndef UCMENU_H
#define UCMENU_H

#include <QQmlListProperty>
#include <QUrl>
#include <QLoggingCategory>
#include <QPointer>
#include <qqml.h>
#include <QtQml/qqml.h>
#include <private/qquickitemchangelistener_p.h>

#include "ucaction.h"
#include "ucactionlist.h"

Q_DECLARE_LOGGING_CATEGORY(ucMenu);

class QPlatformMenu;
class QPlatformMenuItem;
class QQuickItem;
class UCMenuPrivate;
class UCMenuAttached;
class UCMenuBar;
class UCAction;

class UCMenu : public UCAction
{
    Q_OBJECT

    Q_PROPERTY(QQmlListProperty<QObject> data READ data FINAL)
    Q_CLASSINFO("DefaultProperty", "data")

public:
    explicit UCMenu(QObject *parent = 0);
    ~UCMenu();

    QQmlListProperty<QObject> data();

    Q_INVOKABLE void appendObject(QObject* obj);
    Q_INVOKABLE void insertObject(int index, QObject* obj);
    Q_INVOKABLE void removeObject(QObject* obj);

    QPlatformMenu *platformMenu() const;

public Q_SLOTS:
    void show(const QPoint& pt);
    void dismiss();

private:
    Q_DISABLE_COPY(UCMenu)
    Q_DECLARE_PRIVATE(UCMenu)
    QScopedPointer<UCMenuPrivate> d_ptr;

    Q_PRIVATE_SLOT(d_func(), void _q_updateEnabled())
    Q_PRIVATE_SLOT(d_func(), void _q_updateText())
    Q_PRIVATE_SLOT(d_func(), void _q_updateIcon())
    Q_PRIVATE_SLOT(d_func(), void _q_updateVisible())
};

class UCMenuAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(UCMenu* parentMenu READ parentMenu NOTIFY parentMenuChanged)
    Q_PROPERTY(UCMenuBar* menuBar READ menuBar NOTIFY parentMenuChanged)
public:
    explicit UCMenuAttached(QObject *parent);

    static UCMenuAttached *qmlAttachedProperties(QObject *);

    void setParentObject(QObject* o);

    UCMenu *parentMenu() const;
    UCMenuBar *menuBar() const;

Q_SIGNALS:
    void parentMenuChanged();
    void menuBarChanged();

private:
    QPointer<QObject> m_parentObject;
};
QML_DECLARE_TYPEINFO(UCMenuAttached, QML_HAS_ATTACHED_PROPERTIES)


class UCMenuGroup : public UCActionList
{
    Q_OBJECT
public:
    explicit UCMenuGroup(QObject *parent = 0);
};

#endif // UCMENU_H
