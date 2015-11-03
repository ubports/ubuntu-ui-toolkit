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
#include "ucbottomedgesection.h"
#include "propertychange_p.h"
#include <QtQml/private/qqmlproperty_p.h>

/*!
 * \qmltype BottomEdgeSection
 * \instantiates UCBottomEdgeSection
 * \inherits QtObject
 * \inmodule Ubuntu.Components 1.3
 * \since Ubuntu.Components 1.3
 * \ingroup ubuntu
 * \brief Defines an active section within the BottomEdge component.
 *
 * Bottom edge sections are portions within the bottom edge area which can define
 * different content or action whenever the drag enters in the area. The area is
 * defined by \l startsAt and \l endsAt properties, and horizontally is stretched
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
 *             title: "BottomEdge sections"
 *         }
 *
 *         BottomEdge {
 *             height: parent.height - units.gu(20)
 *             hint: BottomEdgeHint {
 *                 text: "My bottom edge"
 *             }
 *             // a fake content till we reach the committable area
 *             contentComponent: Rectangle {
 *                 anchors.fill: parent
 *                 color: UbuntuColors.green
 *             }
 *             // override bottom edge sections to switch to real content
 *             BottomEdgeSection {
 *                 startsAt: 0.33
 *                 contentComponent: Page {
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
 * \l endsAt property differs from 1.0, the bottom edge content will only be exposed
 * to that value, and the \l BottomEdge::state will get the \e SectionCommitted
 * value.
 *
 * \note Whereas there is no restriction on making overlapping sections, beware that
 * overlapping sections changing the content through the \l content or \l contentComponent
 * properties will cause unpredictable results.
 */

UCBottomEdgeSection::UCBottomEdgeSection(QObject *parent)
    : QObject(parent)
    , m_bottomEdge(qobject_cast<UCBottomEdge*>(parent))
    , m_component(Q_NULLPTR)
    , m_urlBackup(Q_NULLPTR)
    , m_componentBackup(Q_NULLPTR)
    , m_startsAt(0.0)
    , m_endsAt(-1.0)
    , m_enabled(true)
    , m_commitToTop(false)
{
    connect(this, &UCBottomEdgeSection::dragEnded,
            this, &UCBottomEdgeSection::onDragEnded, Qt::DirectConnection);
}

void UCBottomEdgeSection::attachToBottomEdge(UCBottomEdge *bottomEdge)
{
    QQml_setParent_noEvent(this, bottomEdge);
    m_bottomEdge = bottomEdge;
    // adjust endsAt property value if not set yet
    if (m_endsAt <= 0.0) {
        m_endsAt = UCBottomEdgePrivate::get(m_bottomEdge)->commitPoint;
        Q_EMIT endsAtChanged();
    }
}

void UCBottomEdgeSection::onDragEnded()
{
    if (!m_bottomEdge) {
        return;
    }
    if (m_endsAt == m_bottomEdge->commitPoint() || m_commitToTop) {
        m_bottomEdge->commit();
    } else {
        // move the bottom edge panel to the endsAt
        UCBottomEdgePrivate::get(m_bottomEdge)->positionPanel(m_endsAt);
    }
}

bool UCBottomEdgeSection::dragInSection(qreal dragRatio)
{
    return (m_enabled && dragRatio >= m_startsAt && dragRatio <= m_endsAt);
}

void UCBottomEdgeSection::enterSection()
{
    Q_EMIT entered();
    if (m_endsAt == m_bottomEdge->commitPoint()) {
        UCBottomEdgePrivate::get(m_bottomEdge)->setState(UCBottomEdge::CanCommit);
    }
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
    qDebug() << "SECTION ENTERED" << objectName();
}

void UCBottomEdgeSection::exitSection()
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
    qDebug() << "SECTION EXITED" << objectName();
}

/*!
 * \qmlproperty bool BottomEdgeSection::enabled
 * Enables the section. Disabled sections do not trigger nor change the BottomEdge
 * content. Defaults to false.
 */

/*!
 * \qmlproperty bool BottomEdgeSection::commitToTop
 * When set, the content specified by the section will be committed to the
 * \l BottomEdge::commitPoint, otherwise it will top at the section's \l endsAt
 * top point. Defaults to false.
 */

/*!
 * \qmlproperty real BottomEdgeSection::startsAt
 * Specifies the starting ratio of the bottom erge area. The value must be bigger
 * or equal to 0 but strictly smaller than \l endsAt. Defaults to 0.0.
 */

/*!
 * \qmlproperty real BottomEdgeSection::endsAt
 * Specifies the ending ratio of the bottom edge area. The value must be bigger
 * than \l startsAt and smaller or equal to \l BottomEdge::commitPoint.
 * \note If the end point is less than the \l BottomEdge::commitPoint, ending the
 * drag within the section will result in exposing the bottom edge content only
 * till the section's end point.
 */

/*!
 * \qmlproperty url BottomEdgeSection::content
 * Specifies the url to the document defining the section specific content. This
 * propery will temporarily override the \l BottomEdge::content property value
 * when the drag gesture enters the section area. The orginal value will be restored
 * once the gesture leaves the section area.
 */

/*!
 * \qmlproperty Component BottomEdgeSection::contentComponent
 * Specifies the component defining the section specific content. This propery
 * will temporarily override the \l BottomEdge::contentComponent property value
 * when the drag gesture enters the section area. The orginal value will be restored
 * once the gesture leaves the section area.
 */

/*!
 * \qmlsignal void BottomEdgeSection::entered()
 * Signal triggered when the drag enters into the area defined by the bottom edge
 * section.
 */

/*!
 * \qmlsignal void BottomEdgeSection::exited()
 * Signal triggered when the drag leaves the area defined by the bottom edge section.
 */

/*!
 * \qmlsignal void BottomEdgeSection::dragEnded()
 * Signal triggered when the drag ends within the active bottom edge section area.
 */
