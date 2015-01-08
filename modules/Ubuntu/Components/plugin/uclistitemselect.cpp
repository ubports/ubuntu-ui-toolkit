/*
 * Copyright 2014 Canonical Ltd.
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

#include "uclistitem.h"
#include "uclistitem_p.h"
#include "uclistitemstyle.h"
#include "listener.h"
#include <QtQml/QQmlInfo>
#include <QtQml/QQmlContext>

UCSelectionHandler::UCSelectionHandler(UCListItem *owner)
    : UCHandlerBase(owner)
    , selected(false)
{
}

void UCSelectionHandler::initialize()
{
    if (!listItem->parentAttached) {
        return;
    }
    connect(listItem->parentAttached, &UCViewItemsAttached::selectModeChanged,
            this, &UCSelectionHandler::setupSelection);

    // set up selection panel if created with selectable enabled
    if (listItem->isSelectable()) {
        setupSelection();
    }
}

void UCSelectionHandler::setSelected(bool value)
{
    if (selected == value) {
        return;
    }
    selected = value;
    // update attached list
    UCListItem *item = listItem->item();
    if (listItem->parentAttached) {
        if (selected) {
            UCViewItemsAttachedPrivate::get(listItem->parentAttached)->addSelectedItem(item);
        } else {
            UCViewItemsAttachedPrivate::get(listItem->parentAttached)->removeSelectedItem(item);
        }
    }
    Q_EMIT item->selectedChanged();
}

void UCSelectionHandler::setupSelection()
{
    // make sure the selection mode panel is prepared; selection panel must take care of the visuals
    bool selectable = listItem->isSelectable();
    if (selectable) {
        listItem->promptRebound();
        bool animate = (senderSignalIndex() >= 0);
        listItem->initStyleItem();
        if (listItem->styleItem && listItem->styleItem->m_selectionDelegate) {
            setupPanel(listItem->styleItem->m_selectionDelegate, animate);
        }
    }
    // and finaly update visuals
    listItem->update();
}
