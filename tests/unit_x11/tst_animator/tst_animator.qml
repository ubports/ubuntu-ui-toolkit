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


/**
 * Test to prevent regressions for bug: https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1338602
 * Activity Indicator crashes in QML/Widget mixed applications
 */

import QtQuick 2.0
import Ubuntu.Components 1.1

Item {
    Flow {
        anchors.fill: parent
        Repeater {
            model: 3

            ActivityIndicator {
                running: true
            }
        }
    }
    
/*   
//Qt Equivalent to ActivityIndcator, enable this to test if Qt still has the problem
    Flow {
        anchors.fill: parent
        Repeater {
            model: 3
            Image {
                id: container

                smooth: true
                visible: true
                fillMode: Image.PreserveAspectFit
                horizontalAlignment: Image.AlignHCenter
                verticalAlignment: Image.AlignVCenter
                source: "../../../src/Ubuntu/Components/Themes/Ambiance/artwork/spinner@8.png"


                RotationAnimator on rotation {
                    running: true
                    from: 0
                    to: 360
                    loops: Animation.Infinite
                    duration: 1000
                }

            }

        }
    }
*/    
}
