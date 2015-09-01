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

#include "uclistitem.h"
#include "uclistitem_p.h"

UCListItemExpansion::UCListItemExpansion(QObject *parent)
    : QObject(parent)
    , m_listItem(static_cast<UCListItem13*>(parent))
    , m_height(0.0)
{
}

bool UCListItemExpansion::expandedWithFlag(UCViewItemsAttached::ExpansionFlag flag)
{
    UCListItemPrivate *listItem = UCListItemPrivate::get(m_listItem);
    UCViewItemsAttachedPrivate *viewItems = UCViewItemsAttachedPrivate::get(listItem->parentAttached);
    return expanded() && (viewItems->expansionFlags & flag);
}

void UCListItemExpansion::enableClickFiltering(bool enable)
{
    UCListItemPrivate *listItem = UCListItemPrivate::get(m_listItem);
    if (enable) {
        listItem->parentAttached->parent()->installEventFilter(this);
    } else {
        listItem->parentAttached->parent()->removeEventFilter(this);
    }
}

bool UCListItemExpansion::expanded()
{
    UCListItemPrivate *listItem = UCListItemPrivate::get(m_listItem);
    UCViewItemsAttachedPrivate *viewItems = UCViewItemsAttachedPrivate::get(listItem->parentAttached);
    return (viewItems && viewItems->expansionList.contains(listItem->index()));
}

void UCListItemExpansion::setExpanded(bool expanded)
{
    if (this->expanded() == expanded) {
        return;
    }
    UCListItemPrivate *listItem = UCListItemPrivate::get(m_listItem);
    UCViewItemsAttachedPrivate *viewItems = UCViewItemsAttachedPrivate::get(listItem->parentAttached);
    if (viewItems) {
        if (viewItems->expansionFlags & UCViewItemsAttached::Exclusive) {
            // collapse all the expanded ones
            viewItems->collapseAll();
        }
        if (expanded) {
            viewItems->expand(listItem->index(), m_listItem);
        } else {
            viewItems->collapse(listItem->index());
        }
    }
    UCListItemPrivate::get(m_listItem)->loadStyleItem();
    // no need to emit changed signal, as ViewItems.expandedIndicesChanged is connected to the change signal
}

void UCListItemExpansion::setHeight(qreal height)
{
    if (m_height == height) {
        return;
    }
    m_height = height;
    Q_EMIT heightChanged();
}
