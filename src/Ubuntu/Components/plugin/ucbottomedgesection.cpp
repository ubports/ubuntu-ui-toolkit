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
#include "ucbottomedgesection.h"

/*!
 * \qmltype BottomEdgeSection
 * \instantiates UCBottomEdgeSection
 * \inherits QtObject
 * \inmodule Ubuntu.Components 1.3
 * \since Ubuntu.Components 1.3
 * \ingroup ubuntu
 * \brief Defines an active section within the BottomEdge component.
 *
 * Bottom edge sections are portions within the bottom edge sensing area which
 * can define different content or action whenever the drag enters in the defined
 * area. The area is defined by \l startsAt and \l endsAt properties, and
 * horizontally it is stretched across bottom edge width.
 *
 * \note Overlapping sections won't work.
 */

UCBottomEdgeSection::UCBottomEdgeSection(QObject *parent)
    : QObject(parent)
    , m_bottomEdge(qobject_cast<UCBottomEdge*>(parent))
    , m_component(Q_NULLPTR)
    , m_startsAt(0.0)
    , m_endsAt(-1.0)
    , m_enabled(true)
{
    connect(this, &UCBottomEdgeSection::dragEnded, &UCBottomEdgeSection::onDragEnded);
}

void UCBottomEdgeSection::onDragEnded()
{
    if (!m_bottomEdge) {
        return;
    }
    if (m_endsAt == m_bottomEdge->m_commitPoint) {
        m_bottomEdge->commit();
    } else {
        // move the bottom edge panel to the endsAt
        m_bottomEdge->positionPanel(m_endsAt);
    }
}

bool UCBottomEdgeSection::dragInSection(qreal dragRatio)
{
    return (m_enabled && dragRatio >= m_startsAt && dragRatio <= m_endsAt);
}

void UCBottomEdgeSection::enterSection()
{
    Q_EMIT entered();
    if (m_endsAt == m_bottomEdge->m_commitPoint) {
        m_bottomEdge->setState(UCBottomEdge::CanCommit);
    }
    qDebug() << "SECTION ENTERED" << objectName();
}

void UCBottomEdgeSection::exitSection()
{
    Q_EMIT exited();
    qDebug() << "SECTION EXITED" << objectName();
}

/*!
 * \qmlproperty bool BottomEdgeSection::enabled
 * Enables the section. Disabled sections do not trigger. Defaults to false.
 */

/*!
 * \qmlproperty bool BottomEdgeSection::commitWhenDragEnds
 * When set, the content will commit to the \l BottomEdge::commitPoint when the
 * drag gesture completes, and \l BottomEdge::commit will be executed. Otherwise
 * the content will commit to the point defined by the \l endsAt.
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
