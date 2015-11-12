/*
 * Copyright 2015 Canonical Ltd.
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
 */

#include "listitemselection.h"
#include "uclistitem_p.h"

ListItemSelection::ListItemSelection(QObject *parent)
    : QObject(parent)
    , hostItem(static_cast<UCListItem*>(parent))
    , selectMode(false)
    , selected(false)
{
    UCListItemPrivate *d = UCListItemPrivate::get(hostItem);
    init(d->parentAttached);
    if (d->parentAttached) {
        selectMode = inSelectMode();
        selected = isSelected();
    }
}

// initialize connections between ViewItems and selection handler
void ListItemSelection::init(UCViewItemsAttached *attached)
{
    if (viewItems) {
        disconnect(viewItems.data(), SIGNAL(selectModeChanged()),
                   this, SLOT(updateSelectMode()));
        disconnect(viewItems.data(), SIGNAL(selectedIndicesChanged(QList<int>)),
                   this, SLOT(updateSelected(QList<int>)));
        viewItems.clear();
    }
    if (attached && (viewItems.data() != attached)) {
        viewItems = attached;
        connect(attached, SIGNAL(selectModeChanged()),
                this, SLOT(updateSelectMode()));
        connect(attached, SIGNAL(selectedIndicesChanged(QList<int>)),
                this, SLOT(updateSelected(QList<int>)));
    }
}

// synchronize selection handler data to the ViewItems
void ListItemSelection::syncToViewItems()
{
    if (viewItems) {
        QSignalBlocker blocker(viewItems.data());
        viewItems->setSelectMode(selectMode);
        if (selected) {
            UCViewItemsAttachedPrivate::get(viewItems.data())->addSelectedItem(hostItem);
        } else {
            UCViewItemsAttachedPrivate::get(viewItems.data())->removeSelectedItem(hostItem);
        }
    }
}

bool ListItemSelection::inSelectMode() const
{
    return viewItems ? viewItems->selectMode() : selectMode;
}
void ListItemSelection::setSelectMode(bool mode)
{
    if (mode == inSelectMode()) {
        return;
    }
    if (viewItems) {
        // selectMode is updated through the updateSelectMode()
        viewItems->setSelectMode(mode);
    } else {
        selectMode = mode;
        Q_EMIT hostItem->selectModeChanged();
    }
}

bool ListItemSelection::isSelected() const
{
    return viewItems ?
                UCViewItemsAttachedPrivate::get(viewItems.data())->isItemSelected(hostItem):
                selected;
}
void ListItemSelection::setSelected(bool selected)
{
    if (selected == isSelected()) {
        return;
    }
    if (viewItems) {
        // this->selected is updated through updateSelected()
        if (selected) {
            UCViewItemsAttachedPrivate::get(viewItems.data())->addSelectedItem(hostItem);
        } else {
            UCViewItemsAttachedPrivate::get(viewItems.data())->removeSelectedItem(hostItem);
        }
    } else {
        this->selected = selected;
        Q_EMIT hostItem->selectedChanged();
    }
}

void ListItemSelection::updateSelectMode()
{
    UCListItemPrivate *d = UCListItemPrivate::get(hostItem);
    selectMode = d->parentAttached->selectMode();
    d->loadStyleItem();
    Q_EMIT hostItem->selectModeChanged();
}

void ListItemSelection::updateSelected(const QList<int> &indices)
{
    UCListItemPrivate *d = UCListItemPrivate::get(hostItem);
    if (selected != indices.contains(d->index())) {
        selected = indices.contains(d->index());
        Q_EMIT hostItem->selectedChanged();
    }
}
