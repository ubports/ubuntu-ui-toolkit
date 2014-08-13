# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2013, 2014 Canonical Ltd.
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

try:
    from unittest import mock
except ImportError:
    import mock

from autopilot.introspection import dbus

import ubuntuuitoolkit
from ubuntuuitoolkit import tests


class QQuickListViewTestCase(tests.QMLStringAppTestCase):

    test_qml = ("""
import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

MainView {
    width: units.gu(48)
    height: units.gu(20)

    Page {

        Column {
            id: column
            width: units.gu(48)
            height: units.gu(20)

            Label {
                id: clickedLabel
                objectName: "clickedLabel"
                text: "No element clicked."
            }

            ListView {
                id: testListView
                objectName: "testListView"
                anchors.left: parent.left
                anchors.right: parent.right
                height: column.height - clickedLabel.paintedHeight
                clip: true
                model: 20

                delegate: ListItem.Standard {
                    objectName: "testListElement%1".arg(index)
                    text: "test list element %1".arg(index)
                    onClicked: clickedLabel.text = objectName
                    height: units.gu(5)
                }
            }
        }
    }
}
""")

    def setUp(self):
        super(QQuickListViewTestCase, self).setUp()
        self.list_view = self.main_view.select_single(
            ubuntuuitoolkit.QQuickListView, objectName='testListView')
        self.label = self.main_view.select_single(
            'Label', objectName='clickedLabel')
        self.assertEqual(self.label.text, 'No element clicked.')

    def test_qquicklistview_custom_proxy_object(self):
        self.assertIsInstance(self.list_view, ubuntuuitoolkit.QQuickListView)

    def test_click_element(self):
        self.list_view.click_element('testListElement0')
        self.assertEqual(self.label.text, 'testListElement0')

    def test_click_element_outside_view_below(self):
        # Click the first element out of view to make sure we are not scrolling
        # to the bottom at once.
        self.assertFalse(
            self.list_view._is_element_clickable('testListElement5'))

        self.list_view.click_element('testListElement5')
        self.assertEqual(self.label.text, 'testListElement5')

    def test_click_element_outside_view_above(self):
        self.list_view.click_element('testListElement9')

        # Click the first element out of view to make sure we are not scrolling
        # to the top at once.
        self.assertFalse(
            self.list_view._is_element_clickable('testListElement4'))

        self.list_view.click_element('testListElement4')
        self.assertEqual(self.label.text, 'testListElement4')

    def test_click_element_not_created_at_start_below(self):
        objectName = 'testListElement19'
        self.assertRaises(
            dbus.StateNotFoundError,
            self.list_view.select_single,
            objectName=objectName)
        self.list_view.click_element(objectName)
        self.assertEqual(self.label.text, 'testListElement19')

    def test_click_element_from_first_page_deleted_when_swiping_down(self):
        """Test that no more swiping is done after finding the element.

        This is a regression test for bug http://pad.lv/1342521 that caused
        us to swipe down after finding an element on the first page of the
        list. If we were lucky, the element was still cashed so we just ended
        up doing two extra swipes. If not, then the element will be deleted
        from the tree and the helper failed.

        """
        # Swipe to the bottom.
        self.list_view.click_element('testListElement19')

        objectName = 'testListElement1'
        self.assertRaises(
            dbus.StateNotFoundError,
            self.list_view.select_single,
            objectName=objectName)
        with mock.patch.object(
                self.list_view,
                'swipe_to_show_more_below') as mock_swipe_down:
            self.list_view.click_element(objectName)
        self.assertFalse(mock_swipe_down.called)
        self.assertEqual(self.label.text, 'testListElement1')

    def test_click_unexisting_element(self):
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException,
            self.list_view.click_element,
            'unexisting')
        self.assertEqual(
            str(error), 'List element with objectName "unexisting" not found.')


class QQuickListViewOutOfViewTestCase(tests.QMLStringAppTestCase):
    """Test that we can make elements visible when the list is out of view."""

    test_qml = ("""
import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

MainView {
    width: units.gu(48)
    height: units.gu(20)

    Page {

        Flickable {

            Column {
                id: column
                width: units.gu(48)
                // The column height is greater than the main view height, so
                // the bottom of the list is out of view.
                height: units.gu(40)

                Label {
                    id: clickedLabel
                    objectName: "clickedLabel"
                    text: "No element clicked."
                }

                ListView {
                    id: testListView
                    objectName: "testListView"
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: column.height - clickedLabel.paintedHeight
                    clip: true
                    model: 20

                    delegate: ListItem.Standard {
                        objectName: "testListElement%1".arg(index)
                        text: "test list element %1".arg(index)
                        onClicked: clickedLabel.text = objectName
                        height: units.gu(5)
                    }
                }
            }
        }
    }
}
""")

    def setUp(self):
        super(QQuickListViewOutOfViewTestCase, self).setUp()
        self.list_view = self.main_view.select_single(
            ubuntuuitoolkit.QQuickListView, objectName='testListView')
        self.label = self.main_view.select_single(
            'Label', objectName='clickedLabel')
        self.assertEqual(self.label.text, 'No element clicked.')

    def test_click_element_outside_view_below(self):
        """Test that we can click an element that's out of view below.

        The list is also out of view, so we must scroll from the bottom of the
        main view.

        """
        # Test for http://pad.lv/1275060.
        # Click the first element out of view to make sure we are not scrolling
        # to the bottom at once.
        self.assertFalse(
            self.list_view._is_element_clickable('testListElement9'))

        self.list_view.click_element('testListElement9')
        self.assertEqual(self.label.text, 'testListElement9')
