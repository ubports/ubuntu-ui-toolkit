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

/*!
 * \qmltype BottomEdge
 * \instantiates UCBottomEdge
 * \inqmlmodule Ubuntu.Components 1.3
 * \inherits Item
 * \ingroup ubuntu
 * \since Ubuntu.Components 1.3
 * \brief A component to handle bottom edge gesture and content.
 *
 * The component provides bottom edge content handling. The bottom egde feature
 * is typically composed of a hint and some content. The content is typically
 * committed (i.e. fully shown) when the drag is completed after it has been
 * dragged more than one third of the screen area. However there can be cases
 * when no content is required and/or the use case requires multiple steps before
 * the content can be fully shown.
 *
 *
 */

/*!
 * \qmlsignal BottomEdge::commitStarted()
 */

/*!
 * \qmlsignal BottomEdge::commitFinished()
 */

/*!
 * \qmlsignal BottomEdge::collapseStarted()
 */

/*!
 * \qmlsignal BottomEdge::collapseFinished()
 */

UCBottomEdge::UCBottomEdge(QQuickItem *parent)
    : QQuickItem(parent)
    , m_hint(Q_NULLPTR)
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

void UCBottomEdge::componentComplete()
{
    QQuickItem::componentComplete();
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
/*!
 * \qmlproperty Item BottomEdge::hint
 * The property holds the component to display the hint for the bottom edge element.
 * The component can be a BottomEdgeHint or any custom component. In case of custom
 * component is used, the component must make sure it implements the states of the
 * BottomEdgeHint component, as that drives the first three \l status flags. BottomEdge
 * automatically takes ownership over the hint set, and will delete the previously set
 * component upon change.
 * \note It is not recommended to change the hint in an ongoing bottom edge swipe.
 */
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
        // connect state change from the hint to keep it in sync
        // we can use lambda as the changed hint component will be deleted
        // so the connection will be removed as well
        connect(m_hint, &QQuickItem::stateChanged, [=](const QString &state) {
            if (state == QStringLiteral("Idle") || state.isEmpty()) {
                setStatus(UCBottomEdge::Idle);
            }
            if (state == QStringLiteral("Hinted")) {
                setStatus(UCBottomEdge::Hinted);
            }
            if (state == QStringLiteral("Faded")) {
                setStatus(UCBottomEdge::Faded);
            }
        });
    }
    Q_EMIT hintChanged();
}

/*!
 * \qmlproperty real BottomEdge::dragProgress
 */

/*!
 * \qmlproperty list<real> BottomEdge::stages
 */
QList<qreal> UCBottomEdge::stages()
{
    return m_stages;
}
/*!
 * \qmlproperty int BottomEdge::currentStageIndex
 */

/*!
 * \qmlproperty enum BottomEdge::status
 * The property reports the actual status of the bottom edge. It can have the
 * following values:
 * \table
 * \header
 *  \li Status
 *  \li Description
 * \row
 *  \li Faded
 *  \li The bottom edge hint is faded out. \l flickable
 * \endtable
 */
void UCBottomEdge::setStatus(UCBottomEdge::Status status)
{
    if (status == m_status) {
        return;
    }
    m_status = status;
    qDebug() << "STATUS" << m_status;
    Q_EMIT statusChanged(m_status);
}

/*!
 * \qmlproperty url BottomEdge::content
 */

/*!
 * \qmlproperty Component BottomEdge::contentComponent
 */

/*!
 * \qmlproperty Item BottomEdge::contentItem
 * \readonly
 */

/*!
 * \qmlmethod void BottomEdge::commit()
 */
void UCBottomEdge::commit()
{

}

/*!
 * \qmlmethod void BottomEdge::collapse()
 */
void UCBottomEdge::collapse()
{

}
