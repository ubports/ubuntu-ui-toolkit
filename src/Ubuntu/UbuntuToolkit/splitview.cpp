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

#include <QtQuick/private/qquickitem_p.h>
#include <QtQuick/private/qquickanchors_p.h>
#include <QtQml/QQmlInfo>

#include "splitview_p.h"
#include "splitview_p_p.h"

#include "privates/splitviewhandler_p_p.h"

UT_NAMESPACE_BEGIN

/*!
 * \qmltype SplitView
 * \inqmlmodule Ubuntu.Components.Labs 1.0
 * \inherits QQuickBasePositioner
 * \ingroup ubuntu-labs
 * \brief A view component with a flexible layout configuration setup.
 *
 * The component arranges the declared child elements horizontally based on an active
 * column configuration layout. Child elements are considered to be views, and each view
 * is identified with a column index, specified by the SplitView.column attached property.
 * Views should not have width declared, because the width of each view is specified
 * by the active layout's configuration (ViewColumn) and will overwrite the value
 * specified by the view.
 *
 * In order a SplitView to show some content it must have at least one active layout
 * present. Views which are not configured by the active layout will be hidden. Hidden
 * views may be resized, therefore if the content is size sensitive (i.e. the amount
 * shown differs on the space available), make sure the content of your view does take
 * this into account.
 * \code
 * import QtQuick 2.4
 * import Ubuntu.Components 1.3
 * import Ubuntu.Components.Labs 1.0
 *
 * MainView {
 *     id: main
 *     width: units.gu(300)
 *     height: units.gu(80)
 *     SplitView {
 *         anchors.fill: parent
 *         layouts: [
 *             SplitViewLayout {
 *                 when: main.width < units.gu(80)
 *                 ViewColumn {
 *                     fillWidth: true
 *                 }
 *             },
 *             SplitViewLayout {
 *                 when: main.width >= units.gu(80)
 *                 ViewColumn {
 *                     minimumWidth: units.gu(30)
 *                     maximumWidth: units.gu(100)
 *                     preferredWidth: units.gu(40)
 *                 }
 *                 ViewColumn {
 *                     minimumWidth: units.gu(40)
 *                     fillWidth: true
 *                 }
 *             }
 *         ]
 *     }
 *
 *     Page {
 *         id: column1
 *         height: parent.height
 *     }
 *     Page {
 *         id: column2
 *         height: parent.height
 *     }
 * }
 * \endcode
 */

/******************************************************************************
 * SplitViewAttached
 */
SplitViewAttached::SplitViewAttached(QObject *parent)
    : QObject(*(new SplitViewAttachedPrivate), parent)
{
}

SplitViewAttached *SplitViewAttached::get(QQuickItem *item)
{
    SplitViewAttached *attached = static_cast<SplitViewAttached*>(
                qmlAttachedPropertiesObject<SplitView>(item, false));
    return attached;
}

ViewColumn *SplitViewAttachedPrivate::getConfig(QQuickItem *attachee)
{
    SplitViewAttached *attached = SplitViewAttached::get(attachee);
    if (!attached) {
        return nullptr;
    }
    return get(attached)->config();
}

void SplitViewAttachedPrivate::configure(SplitView *view, int column)
{
    this->column = column;
    splitView = view;
    Q_EMIT q_func()->columnChanged();
}

void SplitViewAttached::resize(qreal delta)
{
    Q_D(SplitViewAttached);
    ViewColumn *config = d->config();
    if (config) {
        config->resize(delta);
    }
}

/*!
 * \qmlattachedproperty ViewColumn SplitView::columnConfig
 * The attached property holds the active layout's column configuration data.
 */
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

/*!
 * \qmlattachedproperty SplitView SplitView::view
 */

/*!
 * \qmlattachedproperty int SplitView::column
 */

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

// layouts property
/*!
 * \qmlproperty list<SplitViewLayout> SplitView::layouts
 * The property holds the layout configurations declared for the given SplitView.
 * \sa SplitViewLayout
 */
void SplitViewPrivate::layout_Append(QQmlListProperty<SplitViewLayout> *list, SplitViewLayout* layout)
{
    SplitView *view = static_cast<SplitView*>(list->object);
    SplitViewPrivate *d = SplitViewPrivate::get(view);
    d->columnLatouts.append(layout);
    // parent layout to view
    layout->setParent(view);
    // capture layout activation
    QObject::connect(layout, SIGNAL(whenChanged()), view, SLOT(changeLayout()), Qt::DirectConnection);
    Q_EMIT view->layoutsChanged();
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
    Q_EMIT view->layoutsChanged();
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

/*!
 * \qmlproperty SplitViewLayout SplitView::activeLayout
 * \readonly
 * The property holds the active SplitViewLayout instance, or null is no layout
 * is active.
 */
UT_PREPEND_NAMESPACE(SplitViewLayout) *SplitViewPrivate::getActiveLayout()
{
    return activeLayout;
}

// invoked when one of the SplitViewLayouts emits whenChanged()
void SplitViewPrivate::changeLayout()
{
    // go through layouts and check who's the active one
    SplitViewLayout *newActive = nullptr;
    for (SplitViewLayout *layout : columnLatouts) {
        if (SplitViewLayoutPrivate::get(layout)->when) {
            newActive = layout;
            break;
        }
    }
    if (newActive == activeLayout) {
        return;
    }

    // Q: should we reset the sizes of the previous layout?
    // at least it feels  right to preserve the last state of the layout...
    activeLayout = newActive;

    Q_EMIT q_func()->activeLayoutChanged();

    updateLayout();
    if (q_func()->sender()) {
        // was called by a whenChanged() signal invocation
        recalculateWidths(RecalculateAll);
    }
}

void SplitViewPrivate::updateLayout()
{
    Q_Q(SplitView);
    for (QQuickItem *child : q->childItems()) {
        bool visible = true;
        ViewColumn *columnConfig = SplitViewAttachedPrivate::getConfig(child);
        if (!columnConfig) {
            // no configuration for the column, hide it
            visible = false;
        } else {
            ViewColumnPrivate *config = ViewColumnPrivate::get(columnConfig);
            visible = activeLayout
                    && (config->column < SplitViewLayoutPrivate::get(activeLayout)->columnData.size());
        }
        dirty = dirty | (child->isVisible() != visible);
        child->setVisible(visible);
    }
}

void SplitViewPrivate::recalculateWidths(RelayoutOperation operation)
{
    if (!activeLayout || (!QQuickItemPrivate::get(q_func())->componentComplete && !dirty)) {
        return;
    }
    Q_Q(SplitView);

    // remove the spacing from the width
    qreal fillWidth = q->width() - q->spacing() * (SplitViewLayoutPrivate::get(activeLayout)->columnData.size() - 1);
    // stack of columns with fillWidth true
    QList<QQuickItem*> fillStack;

    for (QQuickItem *child : q->childItems()) {
        ViewColumn *columnConfig = SplitViewAttachedPrivate::getConfig(child);
        if (!columnConfig) {
            continue;
        }
        ViewColumnPrivate *config = ViewColumnPrivate::get(columnConfig);

        if (config->fillWidth && !config->resized) {
            // add to the fillWidth stack
            if (operation & CalculateFillWidth) {
                fillStack << child;
            }
        } else {
            if (operation & SetPreferredSize) {
                child->setWidth(config->preferredWidth);
            }
            if (operation & CalculateFillWidth) {
                fillWidth -= config->preferredWidth;
            }
        }
    }

    // split the width between the stacked fillWidth columns
    if (fillStack.size() && (operation & CalculateFillWidth)) {
        fillWidth /= fillStack.size();
        for (QQuickItem *child : fillStack) {
            // even though the column is fillWidth, it may have min and max specified;
            // check if the size can be applied
            ViewColumnPrivate *config = ViewColumnPrivate::get(SplitViewAttachedPrivate::getConfig(child));
            config->setPreferredWidth(fillWidth, false);
            // update preferredWidth so it can be used in case of resize
            child->setWidth(config->preferredWidth);
        }
    }
    dirty = false;
}

/*!
 * \qmlproperty Component SplitView::handleDelegate
 * The property holds the delegate to be shown for the column resizing handle.
 * The delegate is for pure visual, mouse and touch handling should not be provided
 * by it.
 */

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
    d_func()->recalculateWidths(SplitViewPrivate::RecalculateAll);

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
    Q_D(SplitView);
    d->changeLayout();
    QQuickBasePositioner::componentComplete();
}

void SplitView::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickBasePositioner::geometryChanged(newGeometry, oldGeometry);
    // if we have fillWidths, recalculate those!
    // call this on horizontal resize, vertical one will do its job
    if (newGeometry.width() != oldGeometry.width()) {
        Q_D(SplitView);
        d->recalculateWidths(SplitViewPrivate::CalculateFillWidth);
    }
}

void SplitView::itemChange(ItemChange change, const ItemChangeData &data)
{
    QQuickBasePositioner::itemChange(change, data);
    // we must exclude Repeater
    switch (change) {
    case ItemChildAddedChange:
        if (data.item && !data.item->inherits("QQuickRepeater")) {
            // attach properties and configure
            SplitViewAttached *attached = static_cast<SplitViewAttached*>(
                        qmlAttachedPropertiesObject<SplitView>(data.item, true));

            Q_D(SplitView);
            SplitViewAttachedPrivate::get(attached)->configure(this, d->viewCount++);

            // attach the split handler to it
            SplitViewHandler *handler = new SplitViewHandler(data.item);
            handler->connectToView(this);
        }
        break;
    case ItemChildRemovedChange:
        if (data.item && !data.item->inherits("QQuickRepeater")) {
            Q_D(SplitView);
            d->viewCount--;
        }
        break;
    default: // ommit the rest
        break;
    }
}

/******************************************************************
 * properties
 */

void SplitViewPrivate::setHandle(QQmlComponent *delegate)
{
    if (handleDelegate == delegate) {
        return;
    }
    handleDelegate = delegate;
    Q_EMIT q_func()->handleDelegateChanged();
}

UT_NAMESPACE_END

#include "moc_splitview_p.cpp"
