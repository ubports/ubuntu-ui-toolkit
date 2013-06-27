# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2013 Canonical Ltd.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; version 3.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


from UbuntuUiToolkit import tests
from UbuntuUiToolkit import emulators


class MainViewTestCase(tests.UbuntuUiToolkitTestCase):

    test_qml = ("""
import QtQuick 2.0
import Ubuntu.Components 0.1

MainView {
    width: units.gu(48)
    height: units.gu(60)
}
""")

    def test_main_view_custom_emulator(self):
        self.assertIsInstance(self.main_view, emulators.MainView)

    def test_get_header_without_header(self):
        header = self.main_view.get_header()
        self.assertFalse(header.visible)

    def test_toolbar_custom_emulator(self):
        toolbar = self.main_view.get_toolbar()
        self.assertIsInstance(toolbar, emulators.Toolbar)
        self.assertFalse(toolbar.opened)


class PageTestCase(tests.UbuntuUiToolkitTestCase):

    test_qml = ("""
import QtQuick 2.0
import Ubuntu.Components 0.1

MainView {
    width: units.gu(48)
    height: units.gu(60)

    Page {
        title: "Test title"
    }
}
""")

    def test_get_header(self):
        header = self.main_view.get_header()
        self.assertTrue(header.visible)
        self.assertEqual(header.title, "Test title")


class _ToolbarTestCase(tests.UbuntuUiToolkitTestCase):

    test_qml = ("""
import QtQuick 2.0
import Ubuntu.Components 0.1

MainView {
    width: units.gu(50)
    height: units.gu(50)

    Page {
        tools: ToolbarItems {
            ToolbarButton {
                action: Action {
                    text: "button"
                    onTriggered: print("success!")
                }
            }
        locked: false
        opened: false
        }
    }
}
""")

    def test_open_toolbar(self):
        self.assertIsInstance(self.main_view.get_toolbar(), emulators.Toolbar)
