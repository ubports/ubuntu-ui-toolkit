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

#include "ucbottomedgestyle_p.h"
#include "ucbottomedge_p_p.h"

UT_NAMESPACE_BEGIN

/*!
 * \qmltype BottomEdgeStyle
 * \instantiates UCBottomEdgeStyle
 * \inqmlmodule Ubuntu.Components.Styles 1.3
 * \since Ubuntu.Components.Styles 1.3
 * \ingroup style-api
 * \brief Style API for BottomEdge content holder panel.
 */

void UCBottomEdgeStyle::setConsumeMouse(bool consume)
{
    if (consume) {
        setAcceptedMouseButtons(Qt::AllButtons);
    } else {
        setAcceptedMouseButtons(Qt::NoButton);
    }
}

void UCBottomEdgeStyle::mousePressEvent(QMouseEvent *event)
{
    event->accept();
}

/*!
 * \qmlproperty Item BottomEdgeStyle::panel
 * Represents the item holding the bottom edge content.
 */

/*!
 * \qmlproperty Item BottomEdgeStyle::contentItem
 * Holds the current content item created from \l BottomEdge::contentUrl or \l BottomEdge::contentComponent.
 */

/*!
 * \qmlproperty Animation BottomEdgeStyle::panelAnimation
 * Holds the animation of the panel. BottomEdge requires this property to know
 * when the bottom edge is fully committed or collapsed.
 */

/*!
 * \qmlproperty real BottomEdgeStyle::revealThreshold
 * Holds the style configured value which drives when the bottom edge content
 * should be exposed.
 */

UT_NAMESPACE_END
