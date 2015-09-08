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

import QtQuick 2.4
import QtFeedback 5.0
import Ubuntu.Components 1.1

Object {
    readonly property alias enabled: vibra.otherVibrate
    property alias effect: effect

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
