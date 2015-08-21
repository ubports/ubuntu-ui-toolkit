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

#include "uchaptics.h"
#include "plugin.h"
#include <QtQml/QQmlComponent>
#include <QtCore/QUrl>
#include <QDebug>

/*!
  \qmltype Haptics
  \instantiates UCHaptics
  \inqmlmodule Ubuntu.Components 1.1
  \ingroup ubuntu-services
  \brief Singleton defining the haptics feedback used in components, where execution
  of the feedback is controlled by the system settings.

  Supports global feedback as well as custom feedback. Global feedback can be
  configured through its properties, and \l play function will play the default
  configuration, or a custom one if parameter is given.

  Example of using Haptics:
  \qml
  import QtQuick 2.4
  import Ubuntu.Components 1.3

  Item {
      implicitWidth: units.gu(20)
      implicitHeight: units.gu(5)

      Label {
          text: "Press me"
          anchors.fill: parent
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
      }
      MouseArea {
          anchors.fill: parent
          onClicked: Haptics.play()
      }
  }
  \endqml

  Custom effects can be played as follows:
  \qml
  import QtQuick 2.4
  import Ubuntu.Components 1.3

  Item {
      implicitWidth: units.gu(20)
      implicitHeight: units.gu(5)

      Label {
          text: "Press me"
          anchors.fill: parent
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
      }
      MouseArea {
          anchors.fill: parent
          onClicked: Haptics.play({duration: 25, attackIntensity: 0.7})
      }
  }
  \endqml

  \note Though the \l effect property exposes \c start, \c stop and \c pause
  functions, use those only if you want to have feedback independent from what the
  system setting is.
 */

UCHaptics::UCHaptics(QObject *parent)
    : QObject(parent)
{
    connect(&HapticsProxy::instance(), &HapticsProxy::enabledChanged,
            this, &UCHaptics::enabledChanged);
}

/*!
  \qmlproperty bool Haptics::enabled
  \readonly
  The property specifies whether the haptics feedback is enabled or not by the system.
  */
bool UCHaptics::enabled() const
{
    return HapticsProxy::instance().enabled();
}

/*!
  \qmlproperty HapticsEffect Haptics::effect
  \readonly
  The property defines the settings of the haptics effect used by the component.
  The default setting is a haptics effect with a duration of 10 milliseconds
  with an intensity of 1.0, having fading time of 50 millisecods and fading
  intensity 0.0, and attack time of 50 milliseconds and with an intensity of
  0.0.
  */
QObject *UCHaptics::effect() const
{
    return HapticsProxy::instance().effect();
}

/*!
  \qmlmethod void Haptics::play(var customEffect)
  The function plays the feedback with the configuration specified in \l effect
  if no parameter is given. Custom effect can be played by specifying the effect
  properties in a JSON object in \c customEffect.

  The function will exit unconditionaly if playing the effects is blocked by
  system settings.

  The function will not stop any ongoing haptics effect played, if that one
  was a default haptics effect. In case of custom effects, the previous effect
  will be stopped, and settings will be restored before the new haptics will
  be played. The custom settings properties (the ones which are required to
  be different from the ones defined in the \l effect) must be specified in
  the parameter in a JSON object.
  */
void UCHaptics::play(const QVariant &customEffect)
{
    HapticsProxy::instance().play(customEffect);
}

/*********************************************************
 * Proxy implementation
 */
bool HapticsProxy::enabled()
{
    initialize();
    return (m_proxyObject) ? m_proxyObject->property("enabled").toBool() : false;
}

QObject *HapticsProxy::effect()
{
    initialize();
    return (m_proxyObject) ? m_proxyObject->property("effect").value<QObject*>() : Q_NULLPTR;
}

void HapticsProxy::initialize()
{
    if (!m_engine || m_proxyObject) {
        return;
    }
    // load haptics proxy from file system/qrc
    QUrl path = UbuntuComponentsPlugin::pluginUrl().resolved(QUrl("1.1/Haptics.qml"));
    QQmlComponent component(m_engine, path, QQmlComponent::PreferSynchronous);
    if (!component.isError()) {
        m_proxyObject = component.create();
        if (m_proxyObject) {
            connect(m_proxyObject, SIGNAL(enabledChanged()), this, SIGNAL(enabledChanged()));
        }
    } else {
        qWarning() << qPrintable(component.errorString());
    }
}

void HapticsProxy::play(const QVariant &customEffect)
{
    if (!m_engine) {
        qWarning() << "Engine not specified, haptics won't play";
    }
    initialize();
    if (!m_proxyObject) {
        // load haptics proxy from file system/qrc
        QUrl path = UbuntuComponentsPlugin::pluginUrl().resolved(QUrl("1.1/Haptics.qml"));
        QQmlComponent component(m_engine, path, QQmlComponent::PreferSynchronous);
        if (!component.isError()) {
            m_proxyObject = component.create();
        } else {
            qWarning() << qPrintable(component.errorString());
        }
    }
    if (m_proxyObject) {
        // invoke play function
        m_proxyObject->metaObject()->invokeMethod(m_proxyObject, "play", Q_ARG(QVariant, customEffect));
    }
}
