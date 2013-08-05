/*
 * Copyright 2012 Canonical Ltd.
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

#include "inversemouseareatype.h"
#include <QtQuick/QQuickWindow>
#include <QtCore/QEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QInputMethod>
#include <QtGui/QGuiApplication>
#include <QtQuick/private/qquickevents_p_p.h>
#include "quickutils.h"

#include <QDebug>

/*!
  \internal
  \qmltype InverseMouseArea
  \instantiates InverseMouseAreaType
  \inqmlmodule Ubuntu.Components 0.1
  \ingroup ubuntu
  \brief The InverseMouseArea captures mouse events happening outside of a given
  area.

  A typical use case is hiding of a popup or tooltip when the user presses or taps
  outside of the popup or tooltip area. The following example illustrates the use
  of InverseMouseArea in a Popup.

  Popup.qml
  \qml
  import QtQuick 2.0
  import Ubuntu.Components 0.1

  Rectangle {
      anchors.centerIn: parent
      width: 200; height: 200
      color: "darkgray"
      radius: 10
      InverseMouseArea {
         anchors.fill: parent
         acceptedButtons: Qt.LeftButton
         onPressed: parent.destroy()
      }
  }
  \endqml

  In the Popup above the mouse presses happening outside the area will cause
  closing and destroying the popup.

  \qml
  import QtQuick 2.0
  import Ubuntu.Components 0.1

  Item {
      width: 640
      height: 480
      Button {
          id: button
          text: "Press me"
          onClicked: {
              var component = Qt.createComponent("Popup.qml");
              var obj = component.create(parent);
              obj.visible = true;
          }
      }
  }
  \endqml

 */

/*!
  \internal
 */
InverseMouseAreaType::InverseMouseAreaType(QQuickItem *parent) :
    QQuickMouseArea(parent),
    m_ready(false),
    m_sensingArea(QuickUtils::instance().rootItem(this))
{
    QObject::connect(this, SIGNAL(enabledChanged()), this, SLOT(update()));

    if (!m_sensingArea) {
        // get sensing area upon parent change
        QObject::connect(this, SIGNAL(parentChanged(QQuickItem*)), this, SLOT(update()));
    }
}

InverseMouseAreaType::~InverseMouseAreaType()
{
}


/*!
  \internal
  Slot connected to enabledChanged signal to update the state of the handler.
 */
void InverseMouseAreaType::update()
{
    if (!m_ready) {
        return;
    }
    // update sensing area
    if (!m_sensingArea)
        m_sensingArea = QuickUtils::instance().rootItem(this);
}

void InverseMouseAreaType::componentComplete()
{
    QQuickMouseArea::componentComplete();
    // by now the parents shoudl have been resolved so we can look after the
    // topmost component for the sensingArea in case it has not been set yet
    m_ready = true;
    update();
}

/*!
  \qmlproperty Item InverseMouseArea::sensingArea
  This property holds the sensing area of the inverse mouse area. By default it
  is the root item but it can be set to any other area. The area can be reset
  to the root item by setting null to the property.
 */
QQuickItem *InverseMouseAreaType::sensingArea() const
{
    return m_sensingArea;
}
void InverseMouseAreaType::setSensingArea(QQuickItem *sensing)
{
    if (!sensing)
        sensing = QuickUtils::instance().rootItem(this);
    if (sensing != m_sensingArea) {
        m_sensingArea = sensing;
        Q_EMIT sensingAreaChanged();
    }
}

/*
 * Overrides QQuickItem's contains method. Point is in local coordinates.
 */
bool InverseMouseAreaType::contains(const QPointF &point) const
{
    QPointF scenePos = mapToScene(point);
    QRectF oskRect = QGuiApplication::inputMethod()->keyboardRectangle();
    bool pointInArea = QQuickItem::contains(point);
    bool pointInOSK = oskRect.contains(scenePos);
    bool pointOutArea = (m_sensingArea && m_sensingArea->contains(m_sensingArea->mapFromScene(scenePos)));
    return !pointInArea && !pointInOSK && pointOutArea;
}
