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
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#ifndef SPLITVIEW_P_H
#define SPLITVIEW_P_H

#include <QtQuick/private/qquickpositioners_p.h>

#include "ubuntutoolkitglobal.h"

UT_NAMESPACE_BEGIN

class ViewColumnPrivate;
class ViewColumn : public QObject
{
    Q_OBJECT
    Q_PRIVATE_PROPERTY(d_func(), bool fillWidth MEMBER fillWidth)
    Q_PRIVATE_PROPERTY(d_func(), qreal minimumWidth MEMBER minimumWidth)
    Q_PRIVATE_PROPERTY(d_func(), qreal maximumWidth MEMBER maximumWidth)
    Q_PRIVATE_PROPERTY(d_func(), qreal preferredWidth MEMBER preferredWidth)
public:
    explicit ViewColumn(QObject *parent = 0);

private:
    Q_DECLARE_PRIVATE(ViewColumn)
};

class SplitViewLayoutPrivate;
class SplitViewLayout : public QObject
{
    Q_OBJECT

    Q_PRIVATE_PROPERTY(d_func(), bool when MEMBER when NOTIFY whenChanged)
    Q_PRIVATE_PROPERTY(d_func(), QQmlListProperty<ViewColumn> data READ data)
    Q_CLASSINFO("DefaultProperty", "data")
public:
    explicit SplitViewLayout(QObject *parent = 0);

Q_SIGNALS:
    void whenChanged();

private:
    Q_DECLARE_PRIVATE(SplitViewLayout)
};

class SplitViewPrivate;
class SplitView : public QQuickBasePositioner
{
    Q_OBJECT
    Q_PRIVATE_PROPERTY(d_func(), QQmlListProperty<SplitViewLayout> layouts READ layouts)
public:
    explicit SplitView(QQuickItem *parent = 0);

Q_SIGNALS:

public Q_SLOTS:

protected:
    SplitView(SplitViewPrivate &, QQuickItem *);
    ~SplitView();

    // from QQuickBasePositioner
    void doPositioning(QSizeF *contentSize) override;
    void reportConflictingAnchors() override;

private:
    // QQuickBasePositionerPrivate is not an exported API, therefore we cannot derive from it
    SplitViewPrivate* const d_ptr;
    Q_DECLARE_PRIVATE_D(d_ptr, SplitView)
    Q_PRIVATE_SLOT(d_func(), void changeLayout())
};

UT_NAMESPACE_END

QML_DECLARE_TYPE(UT_PREPEND_NAMESPACE(ViewColumn))
QML_DECLARE_TYPE(UT_PREPEND_NAMESPACE(SplitViewLayout))
QML_DECLARE_TYPE(UT_PREPEND_NAMESPACE(SplitView))

#endif // SPLITVIEW_P_H
