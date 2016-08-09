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
 * \brief View column metrics configuration for SplitView.
 *
 * The component provides width metrics configuration for SplitView layout
 * columns. The values are applied on columns by an active SplitViewLayout.
 * On resizing, the values are preserved for the entire lifetime of the
 * component, even when the active layout is changed into an other one.
 * When changed back, the previous values will be used.
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
 * Specifies the minimum width of the column. The number must be a positive value
 * and less or equal than the maximumWidth value.
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
    if (minimumWidth < 0.0) {
        qmlInfo(q_func()) << "minimumWidth cannot be a negative value";
        return;
    }
    minimumWidth = width;
    Q_EMIT q_func()->minimumWidthChanged();
    recalculateLayoutContent();
}

/*!
 * \qmlproperty real ViewColumn::maximumWidth
 * Specifies the maximum width of the column. The number must be a positive value
 * and bigger than the minimumWidth value.
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
    if (maximumWidth < 0.0) {
        qmlInfo(q_func()) << "maximumWidth cannot be a negative value";
        return;
    }
    maximumWidth = width;
    Q_EMIT q_func()->maximumWidthChanged();
    recalculateLayoutContent();
}

/*!
 * \qmlproperty real ViewColumn::preferredWidth
 * The property holds the preferred width of the column. The value must be situated
 * in between minimumWidth and maximumWidth. In case fillWidth is set, the value will
 * hold the actual width of the column, but setting its value will not affect the
 * width of the column.
 */
void ViewColumnPrivate::setPreferredWidth(qreal width, bool notify)
{
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
 * If set, the column width will take the space available after all the other
 * columns with non-fill width are configured. This means that if there is more
 * than one column configured to fill width, the reminder width is split evenly
 * between these columns. If all columns are configured to fill width, the colum
 * widths will be split evenly between all the columns.
 * \note When a column configured with fillWidth is resized, the properties will
 * not be altered, but the fillWidth wioll no longer be taken into account. Instead,
 * the preferredWidth will drive the width of that column from that point on.
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
 *
 * SplitViewLayout defines a layout configuration and the condition when the layout is
 * expected to be applied. In case multiple layout conditions evaluate to true, the first
 * one in the list will be activated.
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
 * The property holds the column configurations for the layout. If the view has more
 * columns than the configuration specifies, the extra columns will be hidden (their
 * visible property will be set to false!). Also, the hidden column sizes may change,
 * therefore size-sensitive content must be aware of this.
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
 * Specifies the condition when to apply the layout. Usually holds a binding which
 * evaluates to true or false to activate the layout.
 */

UT_NAMESPACE_END
