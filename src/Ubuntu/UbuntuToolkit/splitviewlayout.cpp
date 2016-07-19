/* Copyright 2016 Canonical Ltd.
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

#include <QtQuick/private/qquickitem_p.h>
#include <QtQuick/private/qquickanchors_p.h>
#include <QtQml/QQmlInfo>

#include "splitview_p.h"
#include "splitview_p_p.h"

UT_NAMESPACE_BEGIN

static qreal clamp(qreal min, qreal max, qreal v)
{
    return qMax<qreal>(min, qMin<qreal>(max, v));
}

/******************************************************************************
 * ViewColumn configuration object
 */
/*!
 * \qmltype ViewColumn
 * \inmodule Ubuntu.Components.Labs
 * \ingroup ubuntu-labs
 * \brief View column metrics configuration
 */
ViewColumn::ViewColumn(QObject *parent)
    : QObject(*(new ViewColumnPrivate), parent)
{
}

void ViewColumn::componentComplete()
{
    // check preferredWidth, its value may need fixing
    Q_D(ViewColumn);
    d->setPreferredWidth(d->preferredWidth);
    d->completed = true;
}

void ViewColumnPrivate::recalculateLayoutContent()
{
    SplitViewLayout *layout = qobject_cast<SplitViewLayout*>(parent);
    if (!layout) {
        qFatal("ViewColumn declared outside of SplitViewLayout");
    }
    SplitView *view = qobject_cast<SplitView*>(SplitViewLayoutPrivate::get(layout)->parent);
    if (!view) {
        qFatal("SplitViewLayout not used in any SplitView");
    }

    SplitViewPrivate *dView = SplitViewPrivate::get(view);
    if (dView->activeLayout == layout) {
        dView->recalculateWidths(SplitViewPrivate::RecalculateAll);
    }
}

/*!
 * \qmlproperty real ViewColumn::minimumWidth
 */
void ViewColumnPrivate::setMinimumWidth(qreal width)
{
    if (qFuzzyCompare(minimumWidth, width)) {
        return;
    }
    if (minimumWidth > maximumWidth) {
        qmlInfo(q_func()) << "minimumWidth is greater than maximumWidth";
        return;
    }
    minimumWidth = width;
    Q_EMIT q_func()->minimumWidthChanged();
    recalculateLayoutContent();
}

/*!
 * \qmlproperty real ViewColumn::maximumWidth
 */
void ViewColumnPrivate::setMaximumWidth(qreal width)
{
    if (qFuzzyCompare(maximumWidth, width)) {
        return;
    }
    if (maximumWidth < minimumWidth) {
        qmlInfo(q_func()) << "maximumWidth is smaller than minimumWidth";
        return;
    }
    maximumWidth = width;
    Q_EMIT q_func()->maximumWidthChanged();
    recalculateLayoutContent();
}

/*!
 * \qmlproperty real ViewColumn::preferredWidth
 */
void ViewColumnPrivate::setPreferredWidth(qreal width, bool notify)
{
    if (completed && clamp(minimumWidth, maximumWidth, width) != width) {
        qmlInfo(q_func()) << "preferredWidth value falls outside of minimumWidth and maximumWidth values";
        return;
    }
    // clamp
    width = clamp(minimumWidth, maximumWidth, width);
    if (qFuzzyCompare(preferredWidth, width)) {
        return;
    }
    preferredWidth = width;
    if (notify) {
        Q_EMIT q_func()->preferredWidthChanged();
        recalculateLayoutContent();
    }
}

/*!
 * \qmlproperty bool ViewColumn::fillWidth
 */
void ViewColumnPrivate::setFillWidth(bool fill)
{
    if (fill == fillWidth) {
        return;
    }
    fillWidth = fill;
    Q_EMIT q_func()->fillWidthChanged();
    recalculateLayoutContent();
}

bool ViewColumn::resize(qreal delta)
{
    Q_D(ViewColumn);
    // apply limits
    qreal newWidth = d->preferredWidth + delta;
    // clamp
    newWidth = qMax<qreal>(d->minimumWidth, qMin<qreal>(d->maximumWidth, newWidth));
    if (newWidth != d->preferredWidth) {
        d->resized = true;
        d->setPreferredWidth(newWidth);
        return true;
    }
    return false;
}

/******************************************************************************
 * SplitViewLayout layouts configuration
 */
/*!
 * \qmltype SplitViewLayout
 * \inmodule Ubuntu.Components.Labs
 * \ingroup ubuntu-labs
 * \brief Layout configuration for SplitView.
 */
SplitViewLayout::SplitViewLayout(QObject *parent)
    : QObject(*(new SplitViewLayoutPrivate), parent)
{
}

void SplitViewLayoutPrivate::columns_Append(QQmlListProperty<ViewColumn> *list, ViewColumn* data)
{
    SplitViewLayout *layout = static_cast<SplitViewLayout*>(list->object);
    SplitViewLayoutPrivate *d = SplitViewLayoutPrivate::get(layout);
    ViewColumnPrivate::get(data)->column = d->columnData.size();
    // make sure ViewColumn is parented to the layout definition
    data->setParent(layout);
    d->columnData.append(data);
    Q_EMIT layout->columnsChanged();
}
int SplitViewLayoutPrivate::columns_Count(QQmlListProperty<ViewColumn> *list)
{
    SplitViewLayout *layout = static_cast<SplitViewLayout*>(list->object);
    SplitViewLayoutPrivate *d = SplitViewLayoutPrivate::get(layout);
    return d->columnData.size();
}
ViewColumn *SplitViewLayoutPrivate::columns_At(QQmlListProperty<ViewColumn> *list, int index)
{
    SplitViewLayout *layout = static_cast<SplitViewLayout*>(list->object);
    SplitViewLayoutPrivate *d = SplitViewLayoutPrivate::get(layout);
    return d->columnData.at(index);
}
void SplitViewLayoutPrivate::columns_Clear(QQmlListProperty<ViewColumn> *list)
{
    SplitViewLayout *layout = static_cast<SplitViewLayout*>(list->object);
    SplitViewLayoutPrivate *d = SplitViewLayoutPrivate::get(layout);
    qDeleteAll(d->columnData);
    d->columnData.clear();
    Q_EMIT layout->columnsChanged();
}

/*!
 * \qmlproperty list<ViewColumn> SplitViewLayout::columns
 * \default
 */
QQmlListProperty<UT_PREPEND_NAMESPACE(ViewColumn)> SplitViewLayoutPrivate::columns()
{
    Q_Q(SplitViewLayout);
    return QQmlListProperty<UT_PREPEND_NAMESPACE(ViewColumn)>(q, &columnData,
                                                              &columns_Append,
                                                              &columns_Count,
                                                              &columns_At,
                                                              &columns_Clear);
}

/*!
 * \qmlproperty bool SplitViewLayout::when
 * Specifies the condition when to apply the layout.
 */

UT_NAMESPACE_END
