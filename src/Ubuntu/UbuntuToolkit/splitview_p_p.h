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

#ifndef SPLITVIEW_P_P_H
#define SPLITVIEW_P_P_H

#include "splitview_p.h"
#include <QtCore/private/qobject_p.h>

UT_NAMESPACE_BEGIN

class ViewColumnPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(ViewColumn)

public:
    ViewColumnPrivate() {}

    static ViewColumnPrivate *get(ViewColumn *q)
    {
        return q->d_func();
    }

    qreal minimumWidth{0.0};
    qreal maximumWidth{0xFFFFFFFF};
    qreal preferredWidth{0.0};
    int column{-1};
    bool fillWidth{false};
};

class SplitViewLayoutPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(SplitViewLayout)
public:
    SplitViewLayoutPrivate() {}
    static SplitViewLayoutPrivate *get(SplitViewLayout *q)
    {
        return q->d_func();
    }

    QQmlListProperty<UT_PREPEND_NAMESPACE(ViewColumn)> data();

    bool when{false};
    QList<ViewColumn*> columnData;

private:
    static void data_Append(QQmlListProperty<ViewColumn> *, ViewColumn*);
    static int data_Count(QQmlListProperty<ViewColumn> *);
    static ViewColumn *data_At(QQmlListProperty<ViewColumn> *, int);
    static void data_Clear(QQmlListProperty<ViewColumn> *);
};

class SplitViewAttachedPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(SplitViewAttached)
public:
    SplitViewAttachedPrivate() {}

    int column{-1};
};

class SplitViewPrivate
{
    SplitView *const q_ptr{nullptr};
    Q_DECLARE_PUBLIC(SplitView)

public:
    SplitViewPrivate(SplitView *qq);
    virtual ~SplitViewPrivate();

    static SplitViewPrivate *get(SplitView *q)
    {
        return q->d_func();
    }

    QQmlListProperty<UT_PREPEND_NAMESPACE(SplitViewLayout)> layouts();

    QList<SplitViewLayout*> columnLatouts;

    // private slots
    void changeLayout();
private:
    static void layout_Append(QQmlListProperty<SplitViewLayout> *, SplitViewLayout*);
    static int layout_Count(QQmlListProperty<SplitViewLayout> *);
    static SplitViewLayout *layout_At(QQmlListProperty<SplitViewLayout> *, int);
    static void layout_Clear(QQmlListProperty<SplitViewLayout> *);
};

UT_NAMESPACE_END

#endif // SPLITVIEW_P_P_H

