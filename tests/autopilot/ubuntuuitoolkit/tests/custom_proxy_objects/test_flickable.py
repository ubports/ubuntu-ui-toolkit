# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2014 Canonical Ltd.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; version 3.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

import os

import testtools

import ubuntuuitoolkit
from ubuntuuitoolkit import fixture_setup, tests
from ubuntuuitoolkit._custom_proxy_objects import _common


class FlickableTestCase(testtools.TestCase):

    def test_is_flickable_with_flicking_property_must_return_true(self):
        """is_flickable returns True if flickable property exists."""
        dummy_id = (0, 0)
        dummy_flicking = (0, 'dummy')
        state_with_flicking = {'id': dummy_id, 'flicking': dummy_flicking}
        element = _common.UbuntuUIToolkitCustomProxyObjectBase(
            state_with_flicking, 'dummy', 'dummy')
        with element.no_automatic_refreshing():
            self.assertTrue(element.is_flickable())

    def test_is_flickable_without_flicking_property_must_return_false(self):
        """is_flickable returns False if flickable property doesn't exist."""
        dummy_id = (0, 0)
        state_without_flicking = {'id': dummy_id}
        element = _common.UbuntuUIToolkitCustomProxyObjectBase(
            state_without_flicking, 'dummy', 'dummy')
        with element.no_automatic_refreshing():
            self.assertFalse(element.is_flickable())


class IsFlickableTestCase(tests.QMLStringAppTestCase):
    """Functional test to check that is_flickable returns the right value.

    We already have tests for is_flickable with mocks, so here we just check
    with some real elements.

    """

    test_qml = ("""
import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

MainView {
    objectName: 'mainView'
    width: units.gu(48)
    height: units.gu(60)

    Flickable {
        objectName: 'flickable'
    }
    ListView {
        objectName: 'listView'
    }
    Label {
        objectName: 'label'
    }
}
""")

    scenarios = [
        ('main view', dict(object_name='mainView', is_flickable=False)),
        ('flickable', dict(object_name='flickable', is_flickable=True)),
        ('list view', dict(object_name='listView', is_flickable=True)),
        ('label', dict(object_name='label', is_flickable=False))
    ]

    def test_is_flickable(self):
        """Test that is_flickable identifies the elements correctly."""
        element = self.app.select_single(objectName=self.object_name)
        self.assertEqual(element.is_flickable(), self.is_flickable)


class SwipeIntoViewTestCase(tests.QMLStringAppTestCase):

    test_qml = ("""
import QtQuick 2.0
import Ubuntu.Components 0.1

MainView {
    width: units.gu(48)
    height: units.gu(60)

    Label {
        id: clickedLabel
        objectName: "clickedLabel"
        text: "No element clicked."
    }

    Flickable {
        anchors {
            fill: parent
            topMargin: clickedLabel.height
            // It can't be at the bottom, or the toolbar will be opened
            // when we try to click it.
            bottomMargin: units.gu(10)
        }
        objectName: 'flickable'
        height: units.gu(60)
        contentHeight: bottomButton.y + bottomButton.height

        Button {
            id: topButton
            objectName: 'topButton'
            text: 'Top button'
            onClicked: clickedLabel.text = objectName
        }
        Rectangle {
            id: emptyRectangle
            height: units.gu(80)
            anchors.top: topButton.bottom
        }
        Button {
            id: bottomButton
            objectName: 'bottomButton'
            text: 'Bottom button'
            onClicked: clickedLabel.text = objectName
            anchors.top: emptyRectangle.bottom
        }
    }
}
""")

    def setUp(self):
        super(SwipeIntoViewTestCase, self).setUp()
        self.label = self.main_view.select_single(
            'Label', objectName='clickedLabel')
        self.assertEqual(self.label.text, 'No element clicked.')

    def test_swipe_to_bottom(self):
        self.main_view.close_toolbar()

        button = self.main_view.select_single(objectName='bottomButton')
        button.swipe_into_view()

        self.pointing_device.click_object(button)
        self.assertEqual(self.label.text, 'bottomButton')

    def test_swipe_to_top(self):
        self.main_view.close_toolbar()
        bottomButton = self.main_view.select_single(objectName='bottomButton')
        bottomButton.swipe_into_view()

        topButton = self.main_view.select_single(objectName='topButton')
        topButton.swipe_into_view()

        self.pointing_device.click_object(topButton)
        self.assertEqual(self.label.text, 'topButton')


class UnityFlickableTestCase(tests.QMLStringAppTestCase):

    test_qml = ("""
import QtQuick 2.0
import Ubuntu.Components 0.1

MainView {
    width: units.gu(48)
    height: units.gu(60)

    Flickable {
        objectName: 'testFlickable'
        width: 200; height: 200
        contentWidth: image.width; contentHeight: image.height
    }
}
""")

    def launch_application(self):
        fake_application = fixture_setup.FakeApplication(
            qml_file_contents=self.test_qml)
        self.useFixture(fake_application)

        self.app = self.launch_test_application(
            self.get_alternate_launch_command(),
            '-engine',
            fake_application.qml_file_path,
            emulator_base=ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase,
            app_type='qt')

    def get_alternate_launch_command(self):
        root = tests.get_path_to_source_root()
        path_to_local_launcher = os.path.join(
            root, 'tests', 'launcher', 'launcher')
        if os.path.exists(path_to_local_launcher):
            return path_to_local_launcher
        else:
            path_to_installed_launcher = os.path.join(
                '/', 'usr', 'lib', 'ubuntu-ui-toolkit', 'launcher')
        return path_to_installed_launcher

    def test_get_unity_top_container(self):
        """Test that we can get the top cointainer in Unity."""
        # This tests bug http://pad.lv/1314390
        # On Unity, the top container is not the first child as it is in all
        # the apps that have a MainView. This makes the first implementation of
        # _get_top_container fail. Instead of going from the top looking for
        # a container, we should start from the flickable until we find the
        # top-most container.
        test_flickable = self.app.select_single(
            ubuntuuitoolkit.QQuickFlickable, objectName='testFlickable')
        top_container = test_flickable._get_top_container()
        self.assertIsInstance(top_container, ubuntuuitoolkit.MainView)
