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
 *
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#include "ucbottomedgeproxy.h"
#include "ucbottomedgepanel.h"
#include <QtQuick/private/qquickitem_p.h>

UCBottomEdgeProxy::UCBottomEdgeProxy(QQuickItem *parent)
    : QQuickItem(parent)
    , m_status(Idle)
{
}
UCBottomEdgeProxy::~UCBottomEdgeProxy()
{
    UCBottomEdgePanel::instance().removeBottomEdge(this);
}

void UCBottomEdgeProxy::classBegin()
{
    QQuickItem::classBegin();
    UCBottomEdgePanel::instance().initialize(qmlEngine(this));
}

void UCBottomEdgeProxy::itemChange(ItemChange change, const ItemChangeData &data)
{
    if (change == ItemVisibleHasChanged) {
        if (data.boolValue) {
            UCBottomEdgePanel::instance().addBottomEdge(this);
        } else {
            UCBottomEdgePanel::instance().removeBottomEdge(this);
        }
    }
    if (change == ItemParentHasChanged) {
        QQuickAnchors *anchors = QQuickItemPrivate::get(this)->anchors();
        if (data.item) {
            const QQuickAnchorLine left = QQuickItemPrivate::get(data.item)->left();
            const QQuickAnchorLine right = QQuickItemPrivate::get(data.item)->right();
            anchors->setLeft(left);
            anchors->setRight(right);
        } else {
            anchors->resetLeft();
            anchors->resetRight();
        }
    }
    QQuickItem::itemChange(change, data);
}
