# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2013-2015 Canonical Ltd.
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

import ubuntuuitoolkit
from ubuntuuitoolkit import tests


class MainView10TestCase(tests.QMLStringAppTestCase):

    test_qml = ("""
import QtQuick 2.0
import Ubuntu.Components 1.0

MainView {
    width: units.gu(48)
    height: units.gu(60)
    objectName: "mainView"
}
""")

    def test_main_view_custom_proxy_object(self):
        self.assertIsInstance(self.main_view, ubuntuuitoolkit.MainView)

    def test_get_header_without_header(self):
        header = self.main_view.get_header()
        self.assertFalse(header.visible)

    def test_toolbar_custom_proxy_object(self):
        toolbar = self.main_view.get_toolbar()
        self.assertIsInstance(toolbar, ubuntuuitoolkit.Toolbar)

    def test_open_toolbar(self):
        with mock.patch.object(ubuntuuitoolkit.Toolbar, 'open') as mock_open:
            self.main_view.open_toolbar()

        mock_open.assert_called_once_with()

    def test_close_toolbar(self):
        with mock.patch.object(ubuntuuitoolkit.Toolbar, 'close') as mock_close:
            self.main_view.close_toolbar()

        mock_close.assert_called_once_with()

    def test_open_toolbar_returns_the_toolbar(self):
        toolbar = self.main_view.open_toolbar()
        self.assertIsInstance(toolbar, ubuntuuitoolkit.Toolbar)

    def test_get_tabs_without_tabs(self):
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException, self.main_view.get_tabs)
        self.assertEqual(
            str(error), 'The MainView has no Tabs.')

    def test_switch_to_next_tab_without_tabs(self):
        header = self.main_view.get_header()
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException,
            header.switch_to_next_tab)
        self.assertEqual(
            str(error), 'The MainView has no Tabs.')


class MainView12TestCase(tests.QMLStringAppTestCase):

    test_qml = ("""
import QtQuick 2.3
import Ubuntu.Components 1.2

MainView {
    width: units.gu(48)
    height: units.gu(60)
    objectName: "mainView"
}
""")

    def test_main_view_custom_proxy_object(self):
        self.assertIsInstance(self.main_view, ubuntuuitoolkit.MainView)

    def test_get_header_without_header(self):
        header = self.main_view.get_header()
        self.assertFalse(header.visible)

    def test_get_tabs_without_tabs(self):
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException, self.main_view.get_tabs)
        self.assertEqual(
            str(error), 'The MainView has no Tabs.')

    def test_switch_to_next_tab_without_tabs(self):
        header = self.main_view.get_header()
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException,
            header.switch_to_next_tab)
        self.assertEqual(
            str(error), 'The MainView has no Tabs.')

    def test_get_toolbar_without_toolbar(self):
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException, self.main_view.get_toolbar)
        self.assertEqual(
            str(error), 'The MainView has no Toolbar.')

    def test_open_toolbar_without_toolbar(self):
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException, self.main_view.open_toolbar)
        self.assertEqual(
            str(error), 'The MainView has no Toolbar.')

    def test_close_toolbar_without_toolbar(self):
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException, self.main_view.close_toolbar)
        self.assertEqual(
            str(error), 'The MainView has no Toolbar.')


TEST_GO_BACK_QML_FORMAT = ("""
import QtQuick 2.0
import Ubuntu.Components 1.0

MainView {{
    width: units.gu(48)
    height: units.gu(60)
    objectName: "mainView"
    useDeprecatedToolbar: {use_deprecated_toolbar}

    PageStack {{
        id: pageStack
        Component.onCompleted: push(page0)

        Page {{
            id: page0
            title: "Page 0"
            visible: false

            Button {{
                objectName: "go_to_page1"
                text: "Go to page 1"
                onClicked: pageStack.push(page1)
            }}
        }}

        Page {{
            id: page1
            title: "Page 1"
            visible: false
        }}
    }}
}}
""")


class GoBackTestCase(tests.QMLStringAppTestCase):

    scenarios = [
        ('back in toolbar', dict(
            test_qml=TEST_GO_BACK_QML_FORMAT.format(
                use_deprecated_toolbar='true'))),
        ('back in header', dict(
            test_qml=TEST_GO_BACK_QML_FORMAT.format(
                use_deprecated_toolbar='false')))
    ]

    def setUp(self):
        super().setUp()
        self.header = self.main_view.get_header()
        self.assertEqual(self.header.title, 'Page 0')

    def test_open_page(self):
        self._go_to_page1()
        self.assertEqual(self.header.title, 'Page 1')

    def _go_to_page1(self):
        button = self.main_view.select_single(
            'Button',
            objectName='go_to_page1')
        self.pointing_device.click_object(button)
        self.header.wait_for_animation()

    def test_go_back(self):
        self._go_to_page1()
        self.main_view.go_back()
        self.assertEqual(self.header.title, 'Page 0')
