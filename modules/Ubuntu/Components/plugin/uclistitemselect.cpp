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
{
}

void UCSelectionHandler::initialize(bool animated)
{
    UCListItemPrivate *pListItem = UCListItemPrivate::get(listItem);
    if (!pListItem->parentAttached) {
        return;
    }
    connect(pListItem->parentAttached, SIGNAL(selectModeChanged()),
            this, SLOT(setupSelection()));

    // set up selection panel if created with selectable enabled
    if (pListItem->isSelectable()) {
        setupSelection(animated);
    }
}

void UCSelectionHandler::setupSelection(bool animated)
{
    // make sure the selection mode panel is prepared; selection panel must take care of the visuals
    UCListItemPrivate *pListItem = UCListItemPrivate::get(listItem);
    bool selectable = pListItem->isSelectable();
    if (selectable) {
        pListItem->promptRebound();
        bool animate = animated || (senderSignalIndex() >= 0);
        pListItem->initStyleItem();
        if (pListItem->styleItem && pListItem->styleItem->m_selectionDelegate) {
            setupPanel(pListItem->styleItem->m_selectionDelegate, animate);
        }
    }
    // and finaly update visuals
    pListItem->update();
}
