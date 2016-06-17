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

UT_NAMESPACE_BEGIN

ListItemSelection::ListItemSelection(UCListItem *parent)
    : QObject(parent)
    , hostItem(parent)
    , dirtyFlags(0)
    , selectMode(false)
    , selected(false)
{
}

// initialize connections between ViewItems and selection handler
void ListItemSelection::attachToViewItems(UCViewItemsAttached *newViewItems)
{
    if (viewItems.data() == newViewItems) {
        return;
    }
    if (viewItems) {
        disconnect(viewItems.data(), &UCViewItemsAttached::selectModeChanged,
                   this, &ListItemSelection::onSelectModeChanged);
        disconnect(viewItems.data(), &UCViewItemsAttached::selectedIndicesChanged,
                   this, &ListItemSelection::onSelectedIndicesChanged);
        viewItems.clear();
    }
    if (newViewItems) {
        viewItems = newViewItems;
        connect(viewItems.data(), &UCViewItemsAttached::selectModeChanged,
               this, &ListItemSelection::onSelectModeChanged);
        connect(viewItems.data(), &UCViewItemsAttached::selectedIndicesChanged,
                this, &ListItemSelection::onSelectedIndicesChanged);
        syncWithViewItems();
    }
}

// synchronize selection handler data to the ViewItems
void ListItemSelection::syncWithViewItems()
{
    if (viewItems) {
        QSignalBlocker blocker(viewItems.data());

        // sync selectMode
        if (dirtyFlags & SelectModeDirty) {
            viewItems->setSelectMode(selectMode);
        } else if (viewItems->selectMode() != selectMode) {
            selectMode = inSelectMode();
            Q_EMIT hostItem->selectModeChanged();
        }

        // sync selected
        if (dirtyFlags & SelectedDirty) {
            if (selected) {
                UCViewItemsAttachedPrivate::get(viewItems.data())->addSelectedItem(hostItem);
            } else {
                UCViewItemsAttachedPrivate::get(viewItems.data())->removeSelectedItem(hostItem);
            }
        } else if (selected != isSelected()) {
            selected = isSelected();
            Q_EMIT hostItem->selectedChanged();
        }
        dirtyFlags = 0;
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
        dirtyFlags |= SelectModeDirty;
        Q_EMIT hostItem->selectModeChanged();
    }
}

bool ListItemSelection::isSelected() const
{
    if (viewItems) {
        return UCViewItemsAttachedPrivate::get(viewItems.data())->isItemSelected(hostItem);
    }
    return selected;
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
        dirtyFlags |= SelectedDirty;
        Q_EMIT hostItem->selectedChanged();
    }
}

void ListItemSelection::onSelectModeChanged()
{
    UCListItemPrivate *d = UCListItemPrivate::get(hostItem);
    selectMode = d->parentAttached->selectMode();
    d->loadStyleItem();
    Q_EMIT hostItem->selectModeChanged();
}

void ListItemSelection::onSelectedIndicesChanged(const QList<int> &indices)
{
    UCListItemPrivate *d = UCListItemPrivate::get(hostItem);
    if (selected != indices.contains(d->index())) {
        selected = indices.contains(d->index());
        Q_EMIT hostItem->selectedChanged();
    }
}

UT_NAMESPACE_END
