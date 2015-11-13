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
 * Authors: Zsombor Egri <zsombor.egri@canonical.com>
 */

#include "gesturedetector.h"
#include "ucunits.h"
#include <QtCore/QEvent>
#include <QtCore/QRectF>
#include <QtGui/QTouchEvent>
#include <QtQuick/QQuickItem>
#include <QtGui/QGuiApplication>
#include <QtGui/QStyleHints>

#define DETECTION_AREA_THICKNESS_GU 1.2

GestureDetector::GestureDetector(QObject *parent)
    : QObject(parent)
    , m_owner(qobject_cast<QQuickItem*>(parent))
    , m_status(Ready)
    , m_bottomUpSwipeDetected(false)
{
    Q_ASSERT(m_owner);
}
GestureDetector::~GestureDetector()
{
    Q_FOREACH(QObject *object, m_filteredItems) {
        object->removeEventFilter(this);
    }
    m_filteredItems.clear();
}

void GestureDetector::onFilteredItemDeleted(QObject *object)
{
    if (object) {
        object->removeEventFilter(this);
        m_filteredItems.removeAll(object);
    }
}

void GestureDetector::setStatus(Status status)
{
    if (status == m_status) {
        return;
    }
    m_status = status;
    Q_EMIT statusChanged(m_status);
}

bool GestureDetector::isDetecting()
{
    return (m_status > Ready && m_status < Completed);
}

void GestureDetector::setItemFilter(QObject *item)
{
    m_filteredItems.append(item);
    item->installEventFilter(this);
    connect(item, &QObject::destroyed, this, &GestureDetector::onFilteredItemDeleted);
}

void GestureDetector::removeItemFilter(QObject *item)
{
    m_filteredItems.removeAll(item);
    item->removeEventFilter(this);
    disconnect(item, &QObject::destroyed, this, &GestureDetector::onFilteredItemDeleted);
}

bool GestureDetector::handleTouchEvent(QObject *target, QTouchEvent *event)
{
    switch (event->type()) {
    case QEvent::TouchBegin: {
        setStatus(Ready);
        QPointF itemPoint = m_owner->mapFromScene(event->touchPoints()[0].scenePos());
        qreal thickness = UCUnits::instance().gu(DETECTION_AREA_THICKNESS_GU);
        QRectF detectionArea(0.0, m_owner->height() - thickness, m_owner->width(), thickness);
        if (detectionArea.contains(itemPoint)) {
            m_startPoint = itemPoint;
            setStatus(Started);
            if (target == parent()) {
                event->accept();
                return true;
            }
        }
        return false;
    }
    case QEvent::TouchEnd:
    {
        m_startPoint = QPointF();
        setStatus(Completed);
        return false;
    }
    case QEvent::TouchCancel: {
        m_startPoint = QPointF();
        setStatus(Ready);
        return false;
    }
    case QEvent::TouchUpdate: {
        if (m_status == Started) {
            QPointF itemPoint = m_owner->mapFromScene(event->touchPoints()[0].scenePos());
            if (abs(m_startPoint.y() - itemPoint.y()) >= qApp->styleHints()->startDragDistance()) {
                setStatus(Detected);
                Q_EMIT bottomUpSwipeDetected();
            }
        }
        return false;
    }
    default: return false;
    }
}

bool GestureDetector::eventFilter(QObject *target, QEvent *event)
{
    if (m_filteredItems.contains(target)) {
        QEvent::Type type = event->type();
        if (type == QEvent::TouchBegin
                || type == QEvent::TouchUpdate
                || type == QEvent::TouchEnd
                || type == QEvent::TouchCancel) {
            QTouchEvent *touch = static_cast<QTouchEvent*>(event);
            return handleTouchEvent(target, touch);
        } else {
            // pass it on
            return false;
        }
    } else {
        return QObject::eventFilter(target, event);
    }
}
