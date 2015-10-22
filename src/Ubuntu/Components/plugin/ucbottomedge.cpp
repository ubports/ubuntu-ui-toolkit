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

#include "ucbottomedge.h"
#include <QtQml/QQmlEngine>
#include <QtQuick/private/qquickitem_p.h>
#include <QtQuick/private/qquickflickable_p.h>

UCBottomEdge::UCBottomEdge(QQuickItem *parent)
    : QQuickItem(parent)
    , m_hint(Q_NULLPTR)
    , m_flickable(Q_NULLPTR)
    , m_contentComponent(Q_NULLPTR)
    , m_contentItem(Q_NULLPTR)
    , m_panel(Q_NULLPTR)
    , m_dragProgress(0.)
    , m_currentStageIndex(-1)
    , m_status(Idle)
{
}
UCBottomEdge::~UCBottomEdge()
{
}

void UCBottomEdge::classBegin()
{
    QQuickItem::classBegin();
}

void UCBottomEdge::itemChange(ItemChange change, const ItemChangeData &data)
{
    if (change == ItemVisibleHasChanged) {
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

void UCBottomEdge::setHint(QQuickItem *hint)
{
    if (hint == m_hint) {
        return;
    }
    if (m_hint) {
        m_hint->setParentItem(Q_NULLPTR);
        delete m_hint;
        m_hint = Q_NULLPTR;
    }
    m_hint = hint;
    // take ownership
    if (m_hint) {
        QQmlEngine::setObjectOwnership(m_hint, QQmlEngine::CppOwnership);
        m_hint->setParentItem(this);
    }
    Q_EMIT hintChanged();
}

QList<qreal> UCBottomEdge::stages()
{
    return m_stages;
}

void UCBottomEdge::commit()
{

}

void UCBottomEdge::collapse()
{

}
