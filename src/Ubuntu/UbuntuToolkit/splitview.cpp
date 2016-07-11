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

#include "splitview_p.h"
#include "splitview_p_p.h"

UT_NAMESPACE_BEGIN

/******************************************************************************
 * ViewColumn configuration object
 */
ViewColumn::ViewColumn(QObject *parent)
    : QObject(*(new ViewColumnPrivate), parent)
{
}

/******************************************************************************
 * SplitViewLayout layouts configuration
 */
SplitViewLayout::SplitViewLayout(QObject *parent)
    : QObject(*(new SplitViewLayoutPrivate), parent)
{
}

QQmlListProperty<ViewColumn> SplitViewLayoutPrivate::data()
{
    Q_Q(SplitViewLayout);
    return QQmlListProperty<ViewColumn>(q, columnData);
}

/******************************************************************************
 * SplitView
 */
SplitViewPrivate::SplitViewPrivate(SplitView *qq)
    : q_ptr(qq)
{
}

SplitViewPrivate::~SplitViewPrivate()
{
}

void SplitViewPrivate::layout_Append(QQmlListProperty<SplitViewLayout> *list, SplitViewLayout* layout)
{
    SplitView *view = static_cast<SplitView*>(list->object);
    SplitViewPrivate *d = SplitViewPrivate::get(view);
    d->columnLatouts.append(layout);
    // capture layout activation
    QObject::connect(layout, SIGNAL(whenChanged()), view, SLOT(changeLayout()), Qt::DirectConnection);
}
int SplitViewPrivate::layout_Count(QQmlListProperty<SplitViewLayout> *list)
{
    SplitView *view = static_cast<SplitView*>(list->object);
    SplitViewPrivate *d = SplitViewPrivate::get(view);
    return d->columnLatouts.size();
}
SplitViewLayout *SplitViewPrivate::layout_At(QQmlListProperty<SplitViewLayout> *list, int index)
{
    SplitView *view = static_cast<SplitView*>(list->object);
    SplitViewPrivate *d = SplitViewPrivate::get(view);
    return d->columnLatouts.at(index);
}
void SplitViewPrivate::layout_Clear(QQmlListProperty<SplitViewLayout> *list)
{
    SplitView *view = static_cast<SplitView*>(list->object);
    SplitViewPrivate *d = SplitViewPrivate::get(view);
    for (SplitViewLayout *layout : d->columnLatouts) {
        // disconnect layout activation
        QObject::disconnect(layout, SIGNAL(whenChanged()), view, SLOT(changeLayout()));
        delete layout;
    }
    d->columnLatouts.clear();
}
QQmlListProperty<SplitViewLayout> SplitViewPrivate::layouts()
{
    Q_Q(SplitView);
    return QQmlListProperty<SplitViewLayout>(q, (void*)&columnLatouts,
                                             layout_Append,
                                             layout_Count,
                                             layout_At,
                                             layout_Clear);
}

// invoked when one of the SplitViewLayouts emits whenChanged()
void SplitViewPrivate::changeLayout()
{
    // TODO: implement
}

SplitView::SplitView(QQuickItem *parent)
    : QQuickBasePositioner (Horizontal, parent)
    , d_ptr(new SplitViewPrivate(this))
{
}

SplitView::SplitView(SplitViewPrivate &dd, QQuickItem *parent)
    : QQuickBasePositioner(Horizontal, parent)
    , d_ptr(&dd)
{
}

SplitView::~SplitView()
{
    Q_D(SplitView);
    delete d;
}

void SplitView::doPositioning(QSizeF *contentSize)
{
    Q_UNUSED(contentSize);
    // TODO: implement QQuickColumn functionality
}
void SplitView::reportConflictingAnchors()
{
    // TODO: implement QQuickColumn functionality
}

UT_NAMESPACE_END

#include "moc_splitview_p.cpp"
