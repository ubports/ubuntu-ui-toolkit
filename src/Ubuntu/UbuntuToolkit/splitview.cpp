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
#include <QtQuick/private/qquickitem_p.h>
#include <QtQuick/private/qquickanchors_p.h>

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

void SplitViewLayoutPrivate::data_Append(QQmlListProperty<ViewColumn> *list, ViewColumn* data)
{
    SplitViewLayout *layout = static_cast<SplitViewLayout*>(list->object);
    SplitViewLayoutPrivate *d = SplitViewLayoutPrivate::get(layout);
    ViewColumnPrivate::get(data)->column = d->columnData.size();
    d->columnData.append(data);
}
int SplitViewLayoutPrivate::data_Count(QQmlListProperty<ViewColumn> *list)
{
    SplitViewLayout *layout = static_cast<SplitViewLayout*>(list->object);
    SplitViewLayoutPrivate *d = SplitViewLayoutPrivate::get(layout);
    return d->columnData.size();
}
ViewColumn *SplitViewLayoutPrivate::data_At(QQmlListProperty<ViewColumn> *list, int index)
{
    SplitViewLayout *layout = static_cast<SplitViewLayout*>(list->object);
    SplitViewLayoutPrivate *d = SplitViewLayoutPrivate::get(layout);
    return d->columnData.at(index);
}
void SplitViewLayoutPrivate::data_Clear(QQmlListProperty<ViewColumn> *list)
{
    SplitViewLayout *layout = static_cast<SplitViewLayout*>(list->object);
    SplitViewLayoutPrivate *d = SplitViewLayoutPrivate::get(layout);
    qDeleteAll(d->columnData);
    d->columnData.clear();
}
QQmlListProperty<UT_PREPEND_NAMESPACE(ViewColumn)> SplitViewLayoutPrivate::data()
{
    Q_Q(SplitViewLayout);
    return QQmlListProperty<UT_PREPEND_NAMESPACE(ViewColumn)>(q, &columnData,
                                        &data_Append,
                                        &data_Count,
                                        &data_At,
                                        &data_Clear);
}

/******************************************************************************
 * SplitViewAttached
 */
SplitViewAttached::SplitViewAttached(QObject *parent)
    : QObject(*(new SplitViewAttachedPrivate), parent)
{
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

SplitViewAttached *SplitView::qmlAttachedProperties(QObject *owner)
{
    return new SplitViewAttached(owner);
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
                                             &layout_Append,
                                             &layout_Count,
                                             &layout_At,
                                             &layout_Clear);
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
    : QQuickBasePositioner(Vertical, parent)
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
    // Inspired from QtQuick QQuickColumn code
    // FIXME: revisit the code once we move to Qt 5.6 as there were more properties added to positioner

    //Precondition: All items in the positioned list have a valid item pointer and should be positioned
    qreal voffset = 0;

    for (int ii = 0; ii < positionedItems.count(); ++ii) {
        PositionedItem &child = positionedItems[ii];
        positionItemY(voffset, &child);
        contentSize->setWidth(qMax(contentSize->width(), child.item->width()));

        voffset += child.item->height();
        voffset += spacing();
    }

    if (voffset != 0)//If we positioned any items, undo the spacing from the last item
        voffset -= spacing();
    contentSize->setHeight(voffset);
}
void SplitView::reportConflictingAnchors()
{
    // Inspired from QtQuick QQuickColumn code
    bool anchorConflict = false;
    for (int ii = 0; ii < positionedItems.count(); ++ii) {
        const PositionedItem &child = positionedItems.at(ii);
        if (child.item) {
            QQuickAnchors *anchors = QQuickItemPrivate::get(static_cast<QQuickItem *>(child.item))->_anchors;
            if (anchors) {
                QQuickAnchors::Anchors usedAnchors = anchors->usedAnchors();
                if (usedAnchors & QQuickAnchors::TopAnchor ||
                    usedAnchors & QQuickAnchors::BottomAnchor ||
                    usedAnchors & QQuickAnchors::VCenterAnchor ||
                    anchors->fill() || anchors->centerIn()) {
                    anchorConflict = true;
                    break;
                }
            }
        }
    }
    if (anchorConflict) {
        qmlInfo(this) << "Cannot specify top, bottom, verticalCenter, fill or centerIn anchors for items inside SplitView."
            << " SplitView will not function.";
    }
}

UT_NAMESPACE_END

#include "moc_splitview_p.cpp"
