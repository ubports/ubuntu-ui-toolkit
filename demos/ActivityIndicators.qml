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

import QtQuick 1.1
import Ubuntu.Components 0.1

Template {
    title: "Activity Indicator"
    id: indicators

    Column {
        spacing: 5
        Row {
            Button {
                text: (indicator.active) ? "Deactivate" : "Activate"
                onClicked: indicator.active = !indicator.active
            }
            Button {
                text: (indicator.stalled) ? "Resume" : "Stall"
                onClicked: indicator.stalled = !indicator.stalled
            }
        }

        ActivityIndicator {
            id: indicator

            onStarted: echoText.echo("started")
            onStalledChanged: if (stalled) echoText.echo("stalled")
            onResumed: echoText.echo("resumed")
            onFinished: echoText.echo("finished")
        }

        Text {
            id: echoText
            text: "Animation: Inactive"
            function echo(msg) {
                text = "AnimationIndicator: echo \"" + msg +"\""
                console.debug(text)
            }
            Component.onCompleted: echo("ready")
        }
    }
}
