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
        return q ? q->d_func() : nullptr;
    }

    void setMinimumWidth(qreal width);
    void setMaximumWidth(qreal width);
    void setPreferredWidth(qreal width, bool notify = true);
    void setFillWidth(bool fill);

    void recalculateLayoutContent();

    qreal minimumWidth{0.0};
    qreal maximumWidth{0xFFFFFFFF};
    qreal preferredWidth{0.0};
    int column{-1};
    bool fillWidth{false};
    bool resized{false};
    bool completed{false};
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

    QQmlListProperty<UT_PREPEND_NAMESPACE(ViewColumn)> columns();

    QList<ViewColumn*> columnData;
    bool when{false};

private:
    static void columns_Append(QQmlListProperty<ViewColumn> *, ViewColumn*);
    static int columns_Count(QQmlListProperty<ViewColumn> *);
    static ViewColumn *columns_At(QQmlListProperty<ViewColumn> *, int);
    static void columns_Clear(QQmlListProperty<ViewColumn> *);
};

class SplitViewAttachedPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(SplitViewAttached)
public:
    SplitViewAttachedPrivate() {}

    static SplitViewAttachedPrivate *get(SplitViewAttached *q)
    {
        return q->d_func();
    }
    static ViewColumn *getConfig(QQuickItem *attachee);

    UT_PREPEND_NAMESPACE(SplitView*) view() const
    {
        return splitView;
    }
    int getColumn() const
    {
        return column;
    }
    void configure(SplitView *view, int column);
    UT_PREPEND_NAMESPACE(ViewColumn*) config();

    int column{-1};
    SplitView *splitView{nullptr};
};

class SplitViewPrivate
{
    SplitView *const q_ptr{nullptr};
    Q_DECLARE_PUBLIC(SplitView)

public:
    enum RelayoutOperation {
        SetPreferredSize = 0x01,
        CalculateFillWidth = 0x02,
        RecalculateAll = 0xFF
    };

    SplitViewPrivate(SplitView *qq);
    virtual ~SplitViewPrivate();

    static SplitViewPrivate *get(SplitView *q)
    {
        return q->d_func();
    }

    QQmlListProperty<QObject> data();
    QQmlListProperty<UT_PREPEND_NAMESPACE(SplitViewLayout)> layouts();
    UT_PREPEND_NAMESPACE(SplitViewLayout) *getActiveLayout();

    void updateLayout();
    void recalculateWidths(RelayoutOperation operation);
    void setHandle(QQmlComponent *delegate);

    // private slots
    void changeLayout();

    // members
    QList<SplitViewLayout*> columnLatouts;
    SplitViewLayout* activeLayout{nullptr};
    QQmlComponent *handleDelegate{nullptr};
    bool dirty{false};

private:
    static void data_Append(QQmlListProperty<QObject> *, QObject*);

    static void layout_Append(QQmlListProperty<SplitViewLayout> *, SplitViewLayout*);
    static int layout_Count(QQmlListProperty<SplitViewLayout> *);
    static SplitViewLayout *layout_At(QQmlListProperty<SplitViewLayout> *, int);
    static void layout_Clear(QQmlListProperty<SplitViewLayout> *);
};

UT_NAMESPACE_END

#endif // SPLITVIEW_P_P_H

