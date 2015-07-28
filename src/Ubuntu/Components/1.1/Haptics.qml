/*
 * Copyright 2014 Canonical Ltd.
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

pragma Singleton
import QtQuick 2.4
import QtFeedback 5.0
import Ubuntu.Components 1.1

/*!
  \qmltype Haptics
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
  import Ubuntu.Components 1.2

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
  import Ubuntu.Components 1.2

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
  functions, use those only if you want to have feedback independent on what the
  system setting is.
 */
Object {

    /*!
      \qmlproperty bool enabled
      \readonly
      The property specifies whether the haptics feedback is enabled or not by the system.
      */
    readonly property alias enabled: vibra.otherVibrate

    /*!
      \qmlproperty HapticsEffect effect
      The property defines the settings of the haptics effect used by the component.
      The default setting is a haptics effect with a duration of 10 milliseconds
      with an intensity of 1.0, having fading time of 50 millisecods and fading
      intensity 0.0, and attack time of 50 milliseconds and with an intensity of
      0.0.
      */
    property alias effect: effect

    /*!
      \qmlmethod play([customEffect])
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
    function play(customEffect) {
        if (!vibra.otherVibrate) {
            return;
        }
        if (effectData.data) {
            // we have a custom effect playing, stop it
            effect.stop();
        }
        if (effect.running) {
            // this is a global effect, leave
            return;
        }
        if (customEffect) {
            effectData.backup(customEffect);
        }
        effect.start();
    }

    QtObject {
        id: effectData
        property var data

        function backup(customEffect) {
            data = customEffect;
            for (var p in data) {
                var value = data[p];
                data[p] = effect[p];
                effect[p] = value;
            }
        }
        function restore() {
            for (var p in data) {
                effect[p] = data[p];
            }
            data = undefined;
        }
    }

    // local feedback component used to play feedback
    HapticsEffect {
        id: effect
        attackIntensity: 0.0
        attackTime: 50
        intensity: 1.0
        duration: 10
        fadeTime: 50
        fadeIntensity: 0.0

        onStateChanged: {
            if (state == HapticsEffect.Stopped) {
                effectData.restore();
            }
        }
    }

    // watch system settings for otherVibrate
    ServiceProperties {
        objectName: "system_effect_settings"
        id: vibra
        service: "org.freedesktop.Accounts"
        serviceInterface: "org.freedesktop.Accounts"
        path: "/org/freedesktop/Accounts"
        adaptorInterface: "com.ubuntu.touch.AccountsService.Sound"
        property bool otherVibrate: true
    }
}
