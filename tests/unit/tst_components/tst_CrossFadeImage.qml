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
import Ubuntu.Components 1.1

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

    CrossFadeImage {
        id: crossFadeImagePreset
        sourceSize {
            width: 123
            height: 321
        }
        source: Qt.resolvedUrl("demo_image.jpg")
    }

    function loadImage(url) {
        source = url;

        signalSpy.signalName = "statusChanged";

        if (status == Image.Null) {
            signalSpy.clear();
            signalSpy.wait();
        }

        if (status == Image.Loading) {
            signalSpy.clear();
            signalSpy.wait();
        }

        compare(status, Image.Ready, "Image not ready");
    }

    function waitForAnimation() {
        signalSpy.signalName = "runningChanged";

        if (!running) {
            signalSpy.clear();
            signalSpy.wait();
            compare(running, true, "Animation did not start");
        }

        signalSpy.clear();
        signalSpy.wait();
        compare(running, false, "Animation did not stop within 5 seconds.");

    }

    function cleanupTest() {
        fadeDuration = UbuntuAnimation.FastDuration ; // default
        compare(running, false, "Animation is running after testcase");
    }

    function test_fade_data() {
        return [
                    {style: "overlay"},
                    {style: "cross"}
                ];
    }

    function test_fade(data) {
        crossFadeImage.fadeStyle = data.style;
        loadImage("demo_image.jpg");
        loadImage("map_icon.png");
        waitForAnimation();
        cleanupTest();
    }

    function test_fadeDuration_data() {
        return [
                    {style: "overlay"},
                    {style: "cross"}
                ];
    }

    function test_fadeDuration(data) {
        crossFadeImage.fadeStyle = data.style;
        fadeDuration = UbuntuAnimation.SleepyDuration;
        loadImage("demo_image.jpg");
        loadImage("map_icon.png");
        sleep(UbuntuAnimation.SleepyDuration / 2); // < fadeDuration
        compare(running, true, "Animation stopped before 1000 ms");
        waitForAnimation();
        cleanupTest();
    }

    function test_sourceSize() {
        loadImage("demo_image.jpg");
        compare(crossFadeImage.sourceSize.width, 640, "Source width incorrectly initialized.");
        compare(crossFadeImage.sourceSize.height, 427, "Source height incorrectly initialized.");
        crossFadeImage.sourceSize.width = 100;
        crossFadeImage.sourceSize.height = 101;
        compare(crossFadeImage.sourceSize.width, 100, "Source width incorrectly updated.");
        compare(crossFadeImage.sourceSize.height, 101, "Source height incorrectly updated.");
        waitForAnimation();
        cleanupTest();
    }

    function test_sourcePreset() {
        compare(crossFadeImagePreset.sourceSize.width, 123, "Source width incorrectly taken from preset.");
        compare(crossFadeImagePreset.sourceSize.height, 321, "Source height incorrectly take from preset.");
    }
}
