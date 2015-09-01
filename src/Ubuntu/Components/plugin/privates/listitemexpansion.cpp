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
    , m_listItem(static_cast<UCListItem*>(parent))
    , m_height(0)
    , m_expanded(false)
    , m_viewItemsConnected(false)
{
    UCViewItemsAttached *viewItems = UCListItemPrivate::get(m_listItem)->parentAttached;
    if (viewItems) {
        connectExpansion(viewItems);
    }
}

void UCListItemExpansion::connectExpansion(UCViewItemsAttached *viewItems)
{
    if (m_viewItemsConnected) {
        return;
    }
    connect(viewItems, SIGNAL(expandedIndicesChanged()),
            this, SIGNAL(expandedChanged()), Qt::UniqueConnection);
    m_viewItemsConnected = true;
}

void UCListItemExpansion::setExpanded(bool expanded)
{
    if (m_expanded == expanded) {
        return;
    }
    m_expanded = expanded;
    UCListItemPrivate::get(m_listItem)->loadStyleItem();
    Q_EMIT expandedChanged();
}

void UCListItemExpansion::setHeight(qreal height)
{
    if (m_height == height) {
        return;
    }
    m_height = height;
    Q_EMIT heightChanged();
}
