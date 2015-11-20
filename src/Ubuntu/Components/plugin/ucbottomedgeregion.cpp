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
#include "ucbottomedge_p.h"
#include "ucbottomedgeregion.h"
#include "propertychange_p.h"
#include <QtQml/private/qqmlproperty_p.h>

/*!
 * \qmltype BottomEdgeRegion
 * \instantiates UCBottomEdgeRegion
 * \inherits QtObject
 * \inmodule Ubuntu.Components 1.3
 * \since Ubuntu.Components 1.3
 * \ingroup ubuntu
 * \brief Defines an active region within the BottomEdge component.
 *
 * Bottom edge regions are portions within the bottom edge area which can define
 * different content or action whenever the drag enters in the area. The area is
 * defined by \l from and \l to properties, and horizontally is stretched
 * across bottom edge width. Custom content can be defined through \l content or
 * \l contentComponent properties, which will override the \l BottomEdge::content
 * and \l BottomEdge::contentComponent properties for the time the gesture is in
 * the section area.
 * \qml
 * import QtQuick 2.4
 * import Ubuntu.Components 1.3
 *
 * MainView {
 *     width: units.gu(40)
 *     height: units.gu(70)
 *
 *     Page {
 *         header: PageHeader {
 *             title: "BottomEdge regions"
 *         }
 *
 *         BottomEdge {
 *             id: bottomEdge
 *             height: parent.height - units.gu(20)
 *             hint: BottomEdgeHint {
 *                 text: "My bottom edge"
 *             }
 *             // a fake content till we reach the committable area
 *             contentComponent: Rectangle {
 *                 width: bottomEdge.width
 *                 height: bottomEdge.height
 *                 color: UbuntuColors.green
 *             }
 *             // override bottom edge sections to switch to real content
 *             BottomEdgeRegion {
 *                 from: 0.33
 *                 contentComponent: Page {
 *                     width: bottomEdge.width
 *                     height: bottomEdge.height
 *                     header: PageHeader {
 *                         title: "BottomEdge Content"
 *                     }
 *                 }
 *             }
 *         }
 *     }
 * }
 * \endqml
 *
 * Entering into the section area is signalled by the \l entered signal and when
 * drag leaves the area the \l exited signal is emitted. If the drag ends within
 * the section area, the \l dragEnded signal is emitted. In case the section's
 * \l to property is less than 1.0, the bottom edge content will only be exposed
 * to that value, and the \l BottomEdge::state will get the \e Committed value.
 * No further drag is possible after reaching \e Commited state.
 *
 * \note Whereas there is no restriction on making overlapping sections, beware that
 * overlapping sections changing the content through the \l content or \l contentComponent
 * properties will cause unpredictable results.
 */

UCBottomEdgeRegion::UCBottomEdgeRegion(QObject *parent)
    : QObject(parent)
    , m_bottomEdge(qobject_cast<UCBottomEdge*>(parent))
    , m_component(Q_NULLPTR)
    , m_urlBackup(Q_NULLPTR)
    , m_componentBackup(Q_NULLPTR)
    , m_from(0.0)
    , m_to(-1.0)
    , m_enabled(true)
{
    connect(this, &UCBottomEdgeRegion::dragEnded, &UCBottomEdgeRegion::onDragEnded);
}

void UCBottomEdgeRegion::attachToBottomEdge(UCBottomEdge *bottomEdge)
{
    QQml_setParent_noEvent(this, bottomEdge);
    m_bottomEdge = bottomEdge;
    // adjust to property value if not set yet
    if (m_to <= 0.0) {
        m_to = 1.0;
        Q_EMIT toChanged();
    }
}

void UCBottomEdgeRegion::onDragEnded()
{
    if (!m_bottomEdge) {
        return;
    }
    if (m_to == 1.0) {
        m_bottomEdge->commit();
    } else {
        // move the bottom edge panel to the m_to
        m_bottomEdge->commitToRegion(this);
    }
}

bool UCBottomEdgeRegion::contains(qreal dragRatio)
{
    return (m_enabled && dragRatio >= m_from && dragRatio <= m_to);
}

void UCBottomEdgeRegion::enter()
{
    Q_EMIT entered();
    // backup url
    if (m_url.isValid()) {
        m_urlBackup = new PropertyChange(m_bottomEdge, "content");
        QQmlProperty property(this, "content", qmlContext(this));
        QQmlAbstractBinding *binding = QQmlPropertyPrivate::binding(property);
        if (binding) {
            PropertyChange::setBinding(m_urlBackup, binding);
        } else {
            PropertyChange::setValue(m_urlBackup, m_url);
        }
    }
    if (m_component) {
        m_componentBackup = new PropertyChange(m_bottomEdge, "contentComponent");
        QQmlProperty property(this, "contentComponent", qmlContext(this));
        QQmlAbstractBinding *binding = QQmlPropertyPrivate::binding(property);
        if (binding) {
            PropertyChange::setBinding(m_componentBackup, binding);
        } else {
            PropertyChange::setValue(m_componentBackup, QVariant::fromValue<QQmlComponent*>(m_component));
        }
    }
}

void UCBottomEdgeRegion::exit()
{
    if (m_componentBackup) {
        delete m_componentBackup;
        m_componentBackup = Q_NULLPTR;
    }
    if (m_urlBackup) {
        delete m_urlBackup;
        m_urlBackup = Q_NULLPTR;
    }
    Q_EMIT exited();
}

/*!
 * \qmlproperty bool BottomEdgeRegion::enabled
 * Enables the section. Disabled sections do not trigger nor change the BottomEdge
 * content. Defaults to false.
 */

/*!
 * \qmlproperty real BottomEdgeRegion::from
 * Specifies the starting ratio of the bottom erge area. The value must be bigger
 * or equal to 0 but strictly smaller than \l to. Defaults to 0.0.
 */

/*!
 * \qmlproperty real BottomEdgeRegion::to
 * Specifies the ending ratio of the bottom edge area. The value must be bigger
 * than \l from and smaller or equal to 1.0.
 * \note If the end point is less than 1.0, ending the drag within the section
 * will result in exposing the bottom edge content only till the ration specified
 * by this property.
 */

/*!
 * \qmlproperty url BottomEdgeRegion::content
 * Specifies the url to the document defining the section specific content. This
 * propery will temporarily override the \l BottomEdge::content property value
 * when the drag gesture enters the section area. The orginal value will be restored
 * once the gesture leaves the section area.
 */

/*!
 * \qmlproperty Component BottomEdgeRegion::contentComponent
 * Specifies the component defining the section specific content. This propery
 * will temporarily override the \l BottomEdge::contentComponent property value
 * when the drag gesture enters the section area. The orginal value will be restored
 * once the gesture leaves the section area.
 */

/*!
 * \qmlsignal void BottomEdgeRegion::entered()
 * Signal triggered when the drag enters into the area defined by the bottom edge
 * section.
 */

/*!
 * \qmlsignal void BottomEdgeRegion::exited()
 * Signal triggered when the drag leaves the area defined by the bottom edge section.
 */

/*!
 * \qmlsignal void BottomEdgeRegion::dragEnded()
 * Signal triggered when the drag ends within the active bottom edge section area.
 */
