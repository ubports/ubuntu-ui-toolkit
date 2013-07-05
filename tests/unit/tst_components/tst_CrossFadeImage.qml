/*
 * Copyright 2013 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 0.1

TestCase {
    name: "CrossFadeImage"

    property alias status: crossFadeImage.status
    property alias source: crossFadeImage.source
    property alias running: crossFadeImage.running
    property alias fadeDuration: crossFadeImage.fadeDuration

    CrossFadeImage {
        id: crossFadeImage
    }

    SignalSpy {
        id: signalSpy
        target: crossFadeImage
    }

    function loadImage(url) {
        console.log("Loading image...")
        source = url

        signalSpy.signalName = "statusChanged"
        signalSpy.wait()

        if (status == Image.Null) {
            signalSpy.clear()
            signalSpy.wait()
        }

        if (status == Image.Loading) {
            signalSpy.clear()
            signalSpy.wait()
        }

        compare(status, Image.Ready, "Image not ready")
        console.log("Image loaded.")
    }

    function waitForAnimation() {
        signalSpy.signalName = "runningChanged"

        if (!running) {
            signalSpy.clear()
            signalSpy.wait()
            compare(running, true, "Animation did not start")
        }

        signalSpy.clear()
        console.log("Waiting for animation to finish...")
        signalSpy.wait()
        compare(running, false, "Animation did not stop within 5 seconds.")

    }

    function cleanupTest() {
        fadeDuration = 400 // default
        compare(running, false, "Animation is running after testcase")
    }


    function test_fade() {
        loadImage("../../../examples/ubuntu-ui-toolkit-gallery/demo_image.jpg")
        loadImage("../../../examples/ubuntu-ui-toolkit-gallery/map_icon.png")
        waitForAnimation()
        cleanupTest()
    }

    function test_fadeduration() {
        fadeDuration = 1000
        loadImage("../../../examples/ubuntu-ui-toolkit-gallery/demo_image.jpg")
        loadImage("../../../examples/ubuntu-ui-toolkit-gallery/map_icon.png")
        sleep(500) // < fadeDuration
        compare(running, true, "Animation stopped before 1000 ms")
        waitForAnimation()
        cleanupTest()
    }
}
