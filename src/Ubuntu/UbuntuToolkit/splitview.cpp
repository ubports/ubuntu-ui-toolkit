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

void SplitViewAttachedPrivate::configure(SplitView *view, int column)
{
    this->column = column;
    splitView = view;
    Q_EMIT q_func()->columnChanged();
}

UT_PREPEND_NAMESPACE(ViewColumn*) SplitViewAttachedPrivate::config()
{
    if (!splitView || column < 0) {
        return nullptr;
    }
    SplitViewPrivate *d = SplitViewPrivate::get(splitView);
    if (!d->activeLayout) {
        return nullptr;
    }
    if (SplitViewLayoutPrivate::get(d->activeLayout)->columnData.size() <= column) {
        return nullptr;
    }
    return SplitViewLayoutPrivate::get(d->activeLayout)->columnData[column];
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

UT_PREPEND_NAMESPACE(SplitViewAttached) *SplitView::qmlAttachedProperties(QObject *owner)
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
    if (SplitViewLayoutPrivate::get(layout)->when) {
        d->activeLayout = layout;
    }
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
    d->activeLayout = nullptr;
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
    relayout(RecalculateAll);
}

void SplitViewPrivate::relayout(RelayoutOperation operation)
{
    if (!activeLayout || !QQuickItemPrivate::get(q_func())->componentComplete) {
        return;
    }
    Q_Q(SplitView);
    qDebug() << "relayouting";

    // remove the spacing from the width
    qreal fillWidth = q->width() - q->spacing() * (q->childItems().size() - 1);
    // stack of columns with fillWidth true
    QList<QQuickItem*> fillStack;

    for (QQuickItem *child : q->childItems()) {
        SplitViewAttached *attached = static_cast<SplitViewAttached*>(
                    qmlAttachedPropertiesObject<SplitView>(child, false));
        if (!attached) {
            child->setVisible(false);
            continue;
        }

        SplitViewAttachedPrivate *dAttached = SplitViewAttachedPrivate::get(attached);
        // set the item's implicit width to the column's width
        ViewColumnPrivate *columnConfig = ViewColumnPrivate::get(dAttached->config());
        if (!columnConfig) {
            // no configuration for the column, hide it
            child->setVisible(false);
            continue;
        }
        if (columnConfig->fillWidth) {
            // add to the fillWidth stack
            if (operation & CalculateFillWidth) {
                fillStack << child;
            }
        } else {
            if (operation & SetPreferredSize) {
                child->setImplicitWidth(columnConfig->preferredWidth);
            }
            if (operation & CalculateFillWidth) {
                fillWidth -= columnConfig->preferredWidth;
            }
        }
    }

    // split the width between the stacked fillWidth columns
    if (fillStack.size() && (operation & CalculateFillWidth)) {
        fillWidth /= fillStack.size();
        for (QQuickItem *child : fillStack) {
            child->setImplicitWidth(fillWidth);
        }
    }
}

SplitView::SplitView(QQuickItem *parent)
    : QQuickBasePositioner(Horizontal, parent)
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
    // Inspired from QtQuick QQuickRow code
    // FIXME: revisit the code once we move to Qt 5.6 as there were more properties added to positioner

    // calculate the layout before we go into the positioning
    d_func()->relayout(SplitViewPrivate::RecalculateAll);

    //Precondition: All items in the positioned list have a valid item pointer and should be positioned
    QQuickItemPrivate *d = QQuickItemPrivate::get(this);
    qreal hoffset = 0;

    QList<qreal> hoffsets;
    for (int ii = 0; ii < positionedItems.count(); ++ii) {
        PositionedItem &child = positionedItems[ii];

        if (!d->effectiveLayoutMirror) {
            positionItemX(hoffset, &child);
        } else {
            hoffsets << hoffset;
        }

        contentSize->setHeight(qMax(contentSize->height(), child.item->height()));

        hoffset += child.item->width();
        hoffset += spacing();
    }

    if (hoffset != 0) { //If we positioned any items, undo the extra spacing from the last item
        hoffset -= spacing();
    }
    contentSize->setWidth(hoffset);

    if (!d->effectiveLayoutMirror) {
        return;
    }

    //Right to Left layout
    qreal end = 0;
    if (!widthValid()) {
        end = contentSize->width();
    } else {
        end = width();
    }

    int acc = 0;
    for (int ii = 0; ii < positionedItems.count(); ++ii) {
        PositionedItem &child = positionedItems[ii];
        hoffset = end - hoffsets[acc++] - child.item->width();
        positionItemX(hoffset, &child);
    }
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
                 if (usedAnchors & QQuickAnchors::LeftAnchor ||
                     usedAnchors & QQuickAnchors::RightAnchor ||
                     usedAnchors & QQuickAnchors::HCenterAnchor ||
                     anchors->fill() || anchors->centerIn()) {
                     anchorConflict = true;
                     break;
                 }
             }
         }
     }
     if (anchorConflict) {
         qmlInfo(this) << "Cannot specify left, right, horizontalCenter, fill or centerIn anchors for items inside SplitView."
             << " SplitView will not function.";
     }
 }

void SplitView::componentComplete()
{
    QQuickBasePositioner::componentComplete();
}

void SplitView::itemChange(ItemChange change, const ItemChangeData &data)
{
    QQuickBasePositioner::itemChange(change, data);
    if (change == ItemChildAddedChange) {
        // attach properties and configure
        SplitViewAttached *attached = static_cast<SplitViewAttached*>(
                    qmlAttachedPropertiesObject<SplitView>(data.item, true));

        SplitViewAttachedPrivate::get(attached)->configure(this, childItems().size() - 1);
    }
}

void SplitView::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickBasePositioner::geometryChanged(newGeometry, oldGeometry);
    // if we have fillWidths, recalculate those!
    // call this on horizontal resize, vertical one will do its job
    if (newGeometry.width() != oldGeometry.width()) {
        Q_D(SplitView);
        d->relayout(SplitViewPrivate::CalculateFillWidth);
    }
}

qreal SplitView::spacing() const
{
    return 0;
}
void SplitView::setSpacing(qreal s)
{
    qDebug() << "spacing:" << s;
}


UT_NAMESPACE_END

#include "moc_splitview_p.cpp"
