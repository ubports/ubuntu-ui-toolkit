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

#include "ucheader.h"
#include <QDebug>
#include <QtQuick/private/qquickflickable_p.h>
#include <QtQuick/private/qquickanchors_p.h>
#include <QtQuick/private/qquickitem_p.h>

/*!
    \qmltype Header
    \instantiates UCHeader
    \inqmlmodule Ubuntu.Components 1.3
    \ingroup ubuntu
    \brief TODO

    By default, Header will anchor to the left, right and top of its
    parent, and it hides by changing its top-margin to be -height.
    TODO: NOPE. But topMargin will change.
*/

UCHeader::UCHeader(QQuickItem *parent)
//    : UCStyledItemBase(parent)
    : QQuickItem(parent)
//    , m_flickable(Q_NULLPTR)
    , m_flickable_contentY(0)
{
    qDebug() << "Header created!";
}

QQuickFlickable* UCHeader::flickable() {
    return m_flickable.data();
}

void UCHeader::setFlickable(QQuickFlickable *flickable) {
    qDebug() << "Setting flickable.";
    if (m_flickable != flickable) {
        // Finish the current header movement in case the current
        //  flickable is disconnected while moving the header:
        // TODO: internal.movementEndeded()

        qDebug() << "Updating flickable from "<<m_flickable<<" to "<<flickable;
        if (!m_flickable.isNull()) {
            flickable->disconnect(this);
        }
        m_flickable = flickable;

        if (!m_flickable.isNull()) {
            QObject::connect(m_flickable, SIGNAL(contentYChanged()),
                             this, SLOT(_q_scrolledContents()));

            m_flickable_contentY = m_flickable->contentY();
        }

        Q_EMIT flickableChanged();
    }
}

void UCHeader::_q_scrolledContents() {
    qDebug() << "Scrolled flickable contents";
    // Avoid moving the header when rebounding or being dragged over the bounds.
    if (!m_flickable->isAtYBeginning() && !m_flickable->isAtYEnd()) {
        qreal delta_contentY = m_flickable->contentY() - m_flickable_contentY;
        qDebug() << "D = " << delta_contentY;
        QQuickAnchors* anchors = QQuickItemPrivate::get(this)->anchors();
        anchors->setTopMargin(anchors->topMargin() - delta_contentY);
    }
    m_flickable_contentY = m_flickable->contentY();

}
