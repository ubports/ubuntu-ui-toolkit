/*
 * Copyright 2013 Canonical Ltd.
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

import QtQuick 2.2
import QtQuick.Window 2.0
import Ubuntu.Components 1.0

/*!
    \qmltype OrientationHelper
    \inqmlmodule Ubuntu.Components 1.1
    \ingroup ubuntu
    \brief The OrientationHelper automatically rotates its children following the
           orientation of the device.

    Any Item placed inside an Orientation Helper will be automatically rotated
    following the orientation of the device.

    Note that OrientationHelper is always filling its parent (anchors.parent: fill).

    Example:
    \qml
    Item {
        OrientationHelper {
            Label {
                text: "Automatically rotated"
            }
            Button {
                text: "Automatically rotated"
            }
        }
    }
    \endqml
*/
Item {
    id: orientationHelper

    /*!
      Sets whether it will be automatically rotating when the device is.

      The default value is true.

      \qmlproperty bool automaticOrientation
     */
    property bool automaticOrientation: true
    /*!
      Sets whether the rotation transition is performed.

      The default value is true.

      \qmlproperty bool transitionEnabled
     */
    property bool transitionEnabled: true
    /*!
      Exposes whether the orientationTransition is running.

      \qmlproperty alias rotating
     */
    property alias rotating: orientationTransition.running

    /*!
      \qmlproperty int __orientationAngle
      \deprecated

      Use orientationAngle instead.
     */
    property alias __orientationAngle: orientationHelper.orientationAngle

    /*!
      \qmlproperty int orientationAngle

      Calculates the current orientation angle.
     */
    property int orientationAngle: automaticOrientation ? Screen.angleBetween(Screen.primaryOrientation, Screen.orientation) : 0

    /*!
      The property holds if the OrientationHelper should automatically resize the
      contents when the input method appears

      The default value is false.
      */
    property bool anchorToKeyboard: false

    anchors.fill: parent

    Component.onCompleted: orientationTransition.enabled = transitionEnabled

    /*!
      \internal
     */
    onOrientationAngleChanged: internal.applyOrientation()

    /*
      The attached property Screen.orientation is only valid inside Item or
      derived components. Inside Object it evaluates to 0 with no error.
      Also be aware that some apps eg. webbrowser-app set window.contentOrientation
      and thus can hide failure to update it from this code.
      See http://qt-project.org/doc/qt-5.0/qtquick/qml-qtquick-window2-screen.html
    */
    Item {
        id: internal

        /*!
          'window' is defined by QML between startup and showing on the screen.
          There is no signal for when it becomes available and re-declaring it is not safe.

          http://qt-project.org/doc/qt-5.1/qtqml/qml-qtqml2-qt.html
          http://qt-project.org/doc/qt-5.1/qtquick/qmlmodule-qtquick-window2-qtquick-window-2.html
         */
        property bool windowActive: typeof window != 'undefined'

        /*!
          Report the current orientation of the application via QWindow::contentOrientation.
          http://qt-project.org/doc/qt-5.0/qtgui/qwindow.html#contentOrientation-prop
         */
        function applyOrientation() {
            if (windowActive && window)
                window.contentOrientation = Screen.orientation
        }

        onWindowActiveChanged: {
            if (automaticOrientation)
                applyOrientation();
        }

        state: orientationAngle.toString()

        states: [
            State {
                name: "0"
                PropertyChanges {
                    target: orientationHelper
                    rotation: 0
                }
                StateChangeScript {
                    name: "anchorsScript"
                    script: {
                        orientationHelper.anchors.fill = null;
                        orientationHelper.anchors.leftMargin = 0;
                        orientationHelper.anchors.rightMargin = 0;
                        orientationHelper.anchors.topMargin = 0;
                        // this will make sure that the keyboard does not obscure the contents
                        orientationHelper.anchors.bottomMargin = Qt.binding(function() {return Qt.inputMethod.visible && anchorToKeyboard ? Qt.inputMethod.keyboardRectangle.height : 0});
                        orientationHelper.anchors.fill = orientationHelper.parent;
                    }
                }
            },
            State {
                name: "180"
                PropertyChanges {
                    target: orientationHelper
                    rotation: 180
                }
                StateChangeScript {
                    name: "anchorsScript"
                    script: {
                        orientationHelper.anchors.fill = null;
                        orientationHelper.anchors.leftMargin = 0;
                        orientationHelper.anchors.rightMargin = 0;
                        orientationHelper.anchors.topMargin = 0;
                        orientationHelper.anchors.bottomMargin = 0;
                        orientationHelper.anchors.fill = orientationHelper.parent;
                    }
                }
            },
            State {
                name: "270"
                PropertyChanges {
                    target: orientationHelper
                    rotation: 270
                }
                StateChangeScript {
                    name: "anchorsScript"
                    script: {
                        orientationHelper.anchors.fill = null;
                        orientationHelper.anchors.topMargin = Qt.binding(function() {return -(parent.width - parent.height) / 2});
                        orientationHelper.anchors.bottomMargin = Qt.binding(function() {return -(parent.width - parent.height) / 2});
                        orientationHelper.anchors.leftMargin = Qt.binding(function() {return (parent.width - parent.height) / 2});
                        orientationHelper.anchors.rightMargin = Qt.binding(function() {return (parent.width - parent.height) / 2});
                        orientationHelper.anchors.fill = orientationHelper.parent;
                    }
                }
            },
            State {
                name: "90"
                PropertyChanges {
                    target: orientationHelper
                    rotation: 90
                }
                StateChangeScript {
                    name: "anchorsScript"
                    script: {
                        orientationHelper.anchors.fill = null;
                        orientationHelper.anchors.topMargin = Qt.binding(function() {return -(parent.width - parent.height) / 2});
                        orientationHelper.anchors.bottomMargin = Qt.binding(function() {return -(parent.width - parent.height) / 2});
                        orientationHelper.anchors.leftMargin = Qt.binding(function() {return (parent.width - parent.height) / 2});
                        orientationHelper.anchors.rightMargin = Qt.binding(function() {return (parent.width - parent.height) / 2});
                        orientationHelper.anchors.fill = orientationHelper.parent;
                    }
                }
            }
        ]

        transitions: [
            Transition {
                id: orientationTransition
                ParallelAnimation {
                    /* FIXME: this is a workaround for 2 issues that trigger too many changes
                              to the width and height of orientationHelper which creates intermediary
                              states of the UI with unexpected sizes:
                        1) upon state change fast-forwarding is used which means that the final values are computed and applied
                          then immediately reverted before the actual transition is applied
                        2) when margins are applied, width and height are updated separately

                        Without these issues, regular PropertyChanges could be used to set the margins.
                    */
                    ScriptAction {
                        scriptName: "anchorsScript"
                    }
                    RotationAnimator {
                        target: orientationHelper
                        duration: UbuntuAnimation.FastDuration
                        easing: UbuntuAnimation.StandardEasing
                        direction: RotationAnimation.Shortest
                    }
                }
            }
        ]
    }
}
