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
    , m_exposed(true)
    , m_previous_contentY(0)
    , m_flickable(Q_NULLPTR)
{
    qDebug() << "Header created!";
//    m_visible = true;
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

            m_previous_contentY = m_flickable->contentY();
        }
        Q_EMIT flickableChanged();
    }
}

void UCHeader::show() {
    // TODO: animate
    qDebug() << "Showing";
    this->setY(0.0);
//    if (!m_visible) {
        qDebug() << "Setting visible to true";
        m_exposed = true;
//        Q_EMIT openedChanged();
//    }
}

void UCHeader::hide() {
//     TODO: animate
    qDebug() << "Hiding";
    this->setY(-1.0*this->height());
//    if (m_visible) {
        qDebug() << "Setting visible to false";
        m_exposed = false;
//        Q_EMIT openedChanged();
//    }
}

void UCHeader::setExposed(bool exposed) {
    if (exposed) {
        this->show();
    } else {
        this->hide();
    }
}

bool UCHeader::exposed() {
    return m_exposed;
}

void UCHeader::_q_scrolledContents() {
    // Avoid moving the header when rebounding or being dragged over the bounds.
    if (!m_flickable->isAtYBeginning() && !m_flickable->isAtYEnd()) {
        qreal dy = m_flickable->contentY() - m_previous_contentY;
        // Restrict the header y between -height and 0:
        qreal clampedY = qMin(qMax(-this->height(), this->y() - dy), 0.0);
        this->setY(clampedY);
    }
    m_previous_contentY = m_flickable->contentY();
}
