/*
 * Copyright 2013 Canonical Ltd.
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

#include "ullayouts.h"
#include "ullayouts_p.h"
#include "ulconditionallayout.h"
#include "layoutaction_p.h"

ULLayoutsPrivate::ULLayoutsPrivate(ULLayouts *qq)
    : QQmlIncubator(AsynchronousIfNested)
    , q_ptr(qq)
    , currentLayoutItem(0)
    , currentLayoutIndex(-1)
    , completed(false)
{
}

/******************************************************************************
 * ULLayoutsPrivate also acts as QQmlIncubator for the dynamically created layouts.
 * QQmlIncubator stuff
 */
void ULLayoutsPrivate::setInitialState(QObject *object)
{
    Q_Q(ULLayouts);
    // set parent
    object->setParent(q);
    QQuickItem *item = static_cast<QQuickItem*>(object);
    item->setParentItem(q);
    item->setVisible(false);
    item->setEnabled(false);
}

void ULLayoutsPrivate::statusChanged(Status status)
{
    Q_Q(ULLayouts);
    if (status == Ready) {
        // complete layouting
        QQuickItem *previousLayout = currentLayoutItem;

        // reset the layout
        currentLayoutItem = qobject_cast<QQuickItem*>(object());
        Q_ASSERT(currentLayoutItem);

        //reparent components to be laid out
        reparentItems();
        // enable and show layout
        changes << new PropertyChange(currentLayoutItem, "enabled", true)
                << new PropertyChange(currentLayoutItem, "visible", true);
        // apply changes
        changes.apply();
        // clear previous layout
        delete previousLayout;

        Q_EMIT q->currentLayoutChanged();
    }
}

/*
 * QQmlListProperty functions
 */
void ULLayoutsPrivate::append_layout(QQmlListProperty<ULConditionalLayout> *list, ULConditionalLayout *layout)
{
    ULLayouts *_this = static_cast<ULLayouts*>(list->object);
    if (layout) {
        layout->setParent(_this);
        _this->d_ptr->layouts.append(layout);
    }
}

int ULLayoutsPrivate::count_layouts(QQmlListProperty<ULConditionalLayout> *list)
{
    ULLayouts *_this = static_cast<ULLayouts*>(list->object);
    return _this->d_ptr->layouts.count();
}

ULConditionalLayout *ULLayoutsPrivate::at_layout(QQmlListProperty<ULConditionalLayout> *list, int index)
{
    ULLayouts *_this = static_cast<ULLayouts*>(list->object);
    return _this->d_ptr->layouts.at(index);
}

void ULLayoutsPrivate::clear_layouts(QQmlListProperty<ULConditionalLayout> *list)
{
    ULLayouts *_this = static_cast<ULLayouts*>(list->object);
    //FIXME: reset LayoutManager property when removing
    _this->d_ptr->layouts.clear();
}


void ULLayoutsPrivate::listLaidOutItems()
{
    Q_Q(ULLayouts);
    QList<QQuickItem*> items = q->findChildren<QQuickItem*>();
    for (int i = 0; i < items.count(); i++) {
        QQuickItem *item = items[i];
        ULConditionalLayoutAttached *marker = qobject_cast<ULConditionalLayoutAttached*>(
                    qmlAttachedPropertiesObject<ULConditionalLayout>(item, false));
        if (marker && !marker->name().isEmpty()) {
            itemsToLayout.insert(marker->name(), item);
        }
    }
}

void ULLayoutsPrivate::reLayout()
{
    if (!completed || (currentLayoutIndex < 0)) {
        return;
    }
    if (!layouts[currentLayoutIndex]->layout()) {
        return;
    }

    qDebug() << "activate layout:" << q_ptr->currentLayout();
    // redo changes
    changes.revert();

    changes.clear();

    // clear the incubator prior being used
    clear();
    QQmlComponent *component = layouts[currentLayoutIndex]->layout();
    // create using incubation as it may be created asynchronously,
    // case when the attached properties are not yet enumerated
    component->create(*this);
}

void ULLayoutsPrivate::reparentItems()
{
    // create copy of items list, to keep track of which ones we change
    LaidOutItemsMap unusedItems = itemsToLayout;

    // iterate through the Layout definition to find containers - those Items with
    // ConditionalLayout.items set
    QList<QQuickItem*> items = currentLayoutItem->findChildren<QQuickItem*>();
    // check also root element, as it may also be marked as layout section
    items.prepend(currentLayoutItem);

    Q_FOREACH(QQuickItem *container, items) {
        ULConditionalLayoutAttached *fragment = qobject_cast<ULConditionalLayoutAttached*>(
                    qmlAttachedPropertiesObject<ULConditionalLayout>(container, false));
        if (!fragment || fragment->items().isEmpty()) {
            continue;
        }
        Q_FOREACH(const QString &itemName, fragment->items()) {
            // check if we have this item listed to be laid out
            QQuickItem *laidOutItem = unusedItems.value(itemName);
            if (laidOutItem != 0) {
                // reparent and break anchors
                changes << new ParentChange(laidOutItem, container);
                if (container->inherits("QQuickColumn")) {
                    changes << new PropertyChange(laidOutItem, "x", 0.0);
                } else if (container->inherits("QQuickRow")) {
                    changes << new PropertyChange(laidOutItem, "y", 0.0);
                } else if (container->inherits("QQuickFlow") || container->inherits("QQuickGrid")) {
                    changes << new PropertyChange(laidOutItem, "x", 0.0);
                    changes << new PropertyChange(laidOutItem, "y", 0.0);
                }
                changes.addConditionalProperties(laidOutItem, fragment);
                changes << new AnchorChange(laidOutItem);
                // remove from unused ones
                unusedItems.remove(itemName);
            }
        }
    }

    // hide the rest of the unused ones
    QHashIterator<QString, QQuickItem*> i(unusedItems);
    while (i.hasNext()) {
        i.next();
        changes << new PropertyChange(i.value(), "visible", false);
        changes << new PropertyChange(i.value(), "enabled", false);
    }
}


void ULLayoutsPrivate::updateLayout()
{
    if (!completed) {
        return;
    }

    // go through conditions and re-parent for the first valid one
    for (int i = 0; i < layouts.count(); i++) {
        ULConditionalLayout *layout = layouts[i];
        if (!layout->layoutName().isEmpty() && layout->when() && layout->when()->evaluate().toBool()) {
            if (currentLayoutIndex == i) {
                return;
            }
            currentLayoutIndex = i;
            // update layout
            reLayout();
            return;
        }
    }
    // check if we need to switch back to default layout
    if (currentLayoutIndex >= 0) {
        qDebug() << "back to default layout";
        changes.revert();
        changes.clear();
        delete currentLayoutItem;
        currentLayoutItem = 0;
        currentLayoutIndex = -1;
    }
}

/*!
 * \qmltype Layouts
 * \instantiates ULLayouts
 * \inqmlmodule Ubuntu.Layouts 0.1
 * \ingroup ubuntu-layouts
 * \brief Layouts component defines the layouts for different form factors and
 * embeds the components to be laid out.
 *
 * Ideas:
 * - items to be laid out must be direct children of Layouts
 * - ConditionalLayout.name is unique
 * - the order items are laid out in a layout container depends on the order the
 *   names are specified in ConditionalLayout.items list
 */

ULLayouts::ULLayouts(QQuickItem *parent):
    QQuickItem(parent),
    d_ptr(new ULLayoutsPrivate(this))
{
    // By default, QQuickItem does not draw anything. If you subclass
    // QQuickItem to create a visual item, you will need to uncomment the
    // following line and re-implement updatePaintNode()
    
    // setFlag(ItemHasContents, true);
}

ULLayouts::~ULLayouts()
{
}

void ULLayouts::componentComplete()
{
    QQuickItem::componentComplete();
    Q_D(ULLayouts);
    d->completed = true;
    d->listLaidOutItems();
    d->updateLayout();
}

/*!
 * \qmlproperty string Layouts::currentLayout
 * The property holds the active layout name. The default layout is identified
 * by an empty string.
 */

QString ULLayouts::currentLayout() const
{
    Q_D(const ULLayouts);
    return d->currentLayoutIndex >= 0 ? d->layouts[d->currentLayoutIndex]->layoutName() : QString();
}

/*!
 * \internal
 * Provides a list of layouts for internal use.
 */
QList<ULConditionalLayout*> ULLayouts::layoutList()
{
    Q_D(ULLayouts);
    return d->layouts;
}

/*!
 * \qmlproperty list<ConditionalLayout> Layouts::layouts
 * List of different layouts.
 */
QQmlListProperty<ULConditionalLayout> ULLayouts::layouts()
{
    Q_D(ULLayouts);
    return QQmlListProperty<ULConditionalLayout>(this, &(d->layouts),
                                                 &ULLayoutsPrivate::append_layout,
                                                 &ULLayoutsPrivate::count_layouts,
                                                 &ULLayoutsPrivate::at_layout,
                                                 &ULLayoutsPrivate::clear_layouts);
}
