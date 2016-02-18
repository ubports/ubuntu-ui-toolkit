# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2016 Canonical Ltd.
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

from unittest import mock

import testtools
import ubuntuuitoolkit
from ubuntuuitoolkit import (
    tests,
    units,
)
from ubuntuuitoolkit._custom_proxy_objects import _flickable


class FlickableTestCase(testtools.TestCase):

    def test_is_flickable_with_flicking_property_must_return_true(self):
        """is_flickable returns True if flickable property exists."""
        dummy_id = (0, 0)
        dummy_flicking = (0, 'dummy')
        state_with_flicking = {'id': dummy_id, 'flicking': dummy_flicking}
        element = ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase(
            state_with_flicking, '/dummy'.encode(), 'dummy')

        with element.no_automatic_refreshing():
            self.assertTrue(element.is_flickable())

    def test_is_flickable_without_flicking_property_must_return_false(self):
        """is_flickable returns False if flickable property doesn't exist."""
        dummy_id = (0, 0)
        state_without_flicking = {'id': dummy_id}
        element = ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase(
            state_without_flicking, '/dummy'.encode(), 'dummy')

        with element.no_automatic_refreshing():
            self.assertFalse(element.is_flickable())


class IsFlickableTestCase(tests.QMLStringAppTestCase):
    """Functional test to check that is_flickable returns the right value.

    We already have tests for is_flickable with mocks, so here we just check
    with some real elements.

    """

    test_qml = ("""
import QtQuick 2.4
import Ubuntu.Components 1.3

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


# FIXME: Only left-to-right (where x=0 is leftmost) layouts are taken into
#   account. Add support for horizontal flicking with right-to-left layouts.
class SwipeFlickableTestCase(tests.QMLStringAppTestCase):

    test_qml = ("""
import QtQuick 2.4
import Ubuntu.Components 1.3

MainView {
    width: units.gu(48)
    height: units.gu(60)
    objectName: "mainView"

    Label {
        id: clickedLabel
        objectName: "clickedLabel"
        text: "No element clicked."
    }

    Flickable {
        anchors {
            fill: parent
            topMargin: clickedLabel.height
        }
        objectName: 'flickable'
        height: units.gu(60)
        contentHeight: bottomButton.y + bottomButton.height
        contentWidth: topRightButton.x +
            Math.max(topRightButton.width, bottomRightButton.width)

        Button {
            id: topButton
            objectName: 'topButton'
            text: 'Top button'
            onClicked: clickedLabel.text = objectName
        }
        Rectangle {
            id: emptyRectangle
            width: units.gu(70)
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
        Button {
            id: topRightButton
            objectName: 'topRightButton'
            text: 'Top-right button'
            onClicked: clickedLabel.text = objectName
            anchors {
                top: parent.top
                left: emptyRectangle.right
            }
        }
        Button {
            id: bottomRightButton
            objectName: 'bottomRightButton'
            text: 'Bottom-right button'
            onClicked: clickedLabel.text = objectName
            anchors {
                top: emptyRectangle.bottom
                left: emptyRectangle.right
            }
        }
    }
}
""")

    def setUp(self):
        super().setUp()
        self.flickable = self.main_view.select_single(
            ubuntuuitoolkit.QQuickFlickable, objectName='flickable')
        self.label = self.main_view.select_single(objectName='clickedLabel')
        self.assertEqual(self.label.text, 'No element clicked.')

    def test_swipe_into_view_bottom_element(self):
        button = self.main_view.select_single(objectName='bottomButton')
        button.swipe_into_view()

        self.pointing_device.click_object(button)
        self.assertEqual(self.label.text, 'bottomButton')

    def test_swipe_into_view_top_element(self):
        bottomButton = self.main_view.select_single(objectName='bottomButton')
        bottomButton.swipe_into_view()

        topButton = self.main_view.select_single(objectName='topButton')
        topButton.swipe_into_view()

        self.pointing_device.click_object(topButton)
        self.assertEqual(self.label.text, 'topButton')

    def test_swipe_into_view_bottom_right_element(self):
        bottomRightButton = self.main_view.select_single(
            objectName='bottomRightButton')
        bottomRightButton.swipe_into_view()

        self.pointing_device.click_object(bottomRightButton)
        self.assertEqual(self.label.text, 'bottomRightButton')

    def test_swipe_into_view_top_right_element(self):
        topRightButton = self.main_view.select_single(
            objectName='topRightButton')
        topRightButton.swipe_into_view()

        self.pointing_device.click_object(topRightButton)
        self.assertEqual(self.label.text, 'topRightButton')

    def test_swipe_to_top_must_leave_flickable_at_y_beginning(self):
        self.flickable.swipe_to_bottom()
        self.assertFalse(self.flickable.atYBeginning)

        self.flickable.swipe_to_top()
        self.assertTrue(self.flickable.atYBeginning)

    def test_swipe_to_bottom_must_leave_flickable_at_y_end(self):
        self.flickable.swipe_to_top()
        self.assertFalse(self.flickable.atYEnd)

        self.flickable.swipe_to_bottom()
        self.assertTrue(self.flickable.atYEnd)

    def test_swipe_to_leftmost_must_leave_flickable_at_x_beginning(self):
        self.flickable.swipe_to_rightmost()
        self.assertFalse(self.flickable.atXBeginning)

        self.flickable.swipe_to_leftmost()
        self.assertTrue(self.flickable.atXBeginning)

    def test_swipe_to_rightmost_must_leave_flickable_at_x_end(self):
        self.flickable.swipe_to_leftmost()
        self.assertFalse(self.flickable.atXEnd)

        self.flickable.swipe_to_rightmost()
        self.assertTrue(self.flickable.atXEnd)

    def test_swipe_to_show_more_above_with_containers(self):
        """Swipe to show more above must receive containers as parameter."""
        self.flickable.swipe_to_bottom()
        self.assertTrue(self.flickable.atYEnd)

        containers = self.flickable._get_containers()
        self.flickable.swipe_to_show_more_above(containers)
        self.assertFalse(self.flickable.atYEnd)

    def test_swipe_to_show_more_above_without_arguments(self):
        """Calling swipe to show more above must get containers by default."""
        self.flickable.swipe_to_bottom()
        self.assertTrue(self.flickable.atYEnd)

        self.flickable.swipe_to_show_more_above()
        self.assertFalse(self.flickable.atYEnd)

    def test_swipe_to_show_more_below_with_containers(self):
        """Swipe to show more below must receive containers as parameter."""
        self.flickable.swipe_to_top()
        self.assertTrue(self.flickable.atYBeginning)

        containers = self.flickable._get_containers()
        self.flickable.swipe_to_show_more_below(containers)
        self.assertFalse(self.flickable.atYBeginning)

    def test_swipe_to_show_more_below_without_arguments(self):
        """Calling swipe to show more below must get containers by default."""
        self.flickable.swipe_to_top()
        self.assertTrue(self.flickable.atYBeginning)

        self.flickable.swipe_to_show_more_below()
        self.assertFalse(self.flickable.atYBeginning)

    def test_swipe_to_show_more_left_with_containers(self):
        """Swipe to show more left must receive containers as parameter."""
        self.flickable.swipe_to_rightmost()
        self.assertTrue(self.flickable.atXEnd)

        containers = self.flickable._get_containers()
        self.flickable.swipe_to_show_more_left(containers)
        self.assertFalse(self.flickable.atXEnd)

    def test_swipe_to_show_more_left_without_arguments(self):
        """Calling swipe to show more left must get containers by default."""
        self.flickable.swipe_to_rightmost()
        self.assertTrue(self.flickable.atXEnd)

        self.flickable.swipe_to_show_more_left()
        self.assertFalse(self.flickable.atXEnd)

    def test_swipe_to_show_more_right_with_containers(self):
        """Swipe to show more right must receive containers as parameter."""
        self.flickable.swipe_to_leftmost()
        self.assertTrue(self.flickable.atXBeginning)

        containers = self.flickable._get_containers()
        self.flickable.swipe_to_show_more_right(containers)
        self.assertFalse(self.flickable.atXBeginning)

    def test_swipe_to_show_more_right_without_arguments(self):
        """Calling swipe to show more right must get containers by default."""
        self.flickable.swipe_to_leftmost()
        self.assertTrue(self.flickable.atXBeginning)

        self.flickable.swipe_to_show_more_right()
        self.assertFalse(self.flickable.atXBeginning)

    def test_swipe_to_show_more_below_with_bottom_margin(self):
        """Calling swipe to show more below will use the margin in the drag."""
        qquickflickable = self.main_view.select_single(
            ubuntuuitoolkit.QQuickFlickable, objectName='flickable')
        qquickflickable.margin_to_swipe_from_bottom = units.gu(6)
        containers = qquickflickable._get_containers()
        bottom = _flickable._get_visible_container_bottom(containers)

        with mock.patch.object(
                qquickflickable.pointing_device, 'drag') as mock_drag:
            try:
                qquickflickable.swipe_to_show_more_below()
            except ubuntuuitoolkit.ToolkitException:
                # An exception will be raised because the drag was faked.
                pass

        mock_drag.assert_called_with(
            mock.ANY, bottom - units.gu(6), mock.ANY, mock.ANY, rate=mock.ANY)

    def test_swipe_to_show_more_above_with_top_margin(self):
        """Calling swipe to show more above will use the margin in the drag."""
        qquickflickable = self.main_view.select_single(
            ubuntuuitoolkit.QQuickFlickable, objectName='flickable')
        qquickflickable.margin_to_swipe_from_top = units.gu(6)
        containers = qquickflickable._get_containers()
        top = _flickable._get_visible_container_top(containers)

        qquickflickable.swipe_to_bottom()
        with mock.patch.object(
                qquickflickable.pointing_device, 'drag') as mock_drag:
            try:
                qquickflickable.swipe_to_show_more_above()
            except ubuntuuitoolkit.ToolkitException:
                # An exception will be raised because the drag was faked.
                pass

        mock_drag.assert_called_with(
            mock.ANY, top + units.gu(6), mock.ANY, mock.ANY, rate=mock.ANY)

    def test_swipe_to_show_more_right_with_right_margin(self):
        """Calling swipe to show more right will use the margin in the drag."""
        qquickflickable = self.main_view.select_single(
            ubuntuuitoolkit.QQuickFlickable, objectName='flickable')
        qquickflickable.margin_to_swipe_from_right = units.gu(6)
        containers = qquickflickable._get_containers()
        rightmost = _flickable._get_visible_container_rightmost(containers)

        with mock.patch.object(
                qquickflickable.pointing_device, 'drag') as mock_drag:
            try:
                qquickflickable.swipe_to_show_more_right()
            except ubuntuuitoolkit.ToolkitException:
                # An exception will be raised because the drag was faked.
                pass

        mock_drag.assert_called_with(
            rightmost - units.gu(6), mock.ANY, mock.ANY, mock.ANY,
            rate=mock.ANY)

    def test_swipe_to_show_more_left_with_left_margin(self):
        """Calling swipe to show more above will use the margin in the drag."""
        qquickflickable = self.main_view.select_single(
            ubuntuuitoolkit.QQuickFlickable, objectName='flickable')
        qquickflickable.margin_to_swipe_from_left = units.gu(6)
        containers = qquickflickable._get_containers()
        leftmost = _flickable._get_visible_container_leftmost(containers)

        qquickflickable.swipe_to_rightmost()
        with mock.patch.object(
                qquickflickable.pointing_device, 'drag') as mock_drag:
            try:
                qquickflickable.swipe_to_show_more_left()
            except ubuntuuitoolkit.ToolkitException:
                # An exception will be raised because the drag was faked.
                pass

        mock_drag.assert_called_with(
            leftmost + units.gu(6), mock.ANY, mock.ANY, mock.ANY,
            rate=mock.ANY)

    def test_failed_drag_must_raise_exception(self):
        dummy_coordinates = (0, 0, 10, 10)
        # Patch the pointing device so it does nothing and the swipe fails.
        with mock.patch.object(self.flickable, 'pointing_device'):
            error = self.assertRaises(
                ubuntuuitoolkit.ToolkitException,
                self.flickable._slow_drag,
                *dummy_coordinates
            )

        self.assertEqual('Could not swipe in the flickable.', str(error))


class UnityFlickableTestCase(tests.QMLStringAppTestCase):

    test_qml = ("""
import QtQuick 2.4
import Ubuntu.Components 1.3

MainView {
    width: units.gu(48)
    height: units.gu(60)
    objectName: 'mainView'

    Flickable {
        objectName: 'testFlickable'
        width: 200; height: 200
        contentWidth: image.width; contentHeight: image.height
    }
}
""")

    def get_command_line(self, command_line):
        command_line.append('-engine')
        return command_line

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
