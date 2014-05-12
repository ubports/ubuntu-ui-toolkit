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

import ubuntuuitoolkit
from ubuntuuitoolkit import tests


class HeaderTestCase(tests.QMLStringAppTestCase):

    test_qml = ("""
import QtQuick 2.0
import Ubuntu.Components 0.1

MainView {
    width: units.gu(48)
    height: units.gu(60)

    useDeprecatedToolbar: false

    Page {
        title: "Test title"

        Label {
            id: label
            objectName: "clicked_label"
            anchors.centerIn: parent
            text: "No button clicked."
        }

        tools: ToolbarItems {
            back: ToolbarButton {
                action: Action {
                    iconName: "cancel"
                    text: "cancel"
                    onTriggered: label.text = "Cancel button clicked."
                }
            }
            Repeater {
                model: 5
                ToolbarButton {
                    action: Action {
                        objectName: "action" + index
                        text: "text " + index
                        iconName: "add"
                        onTriggered: label.text = "Button "+index+" clicked."
                    }
                }
            }
        }
    }
}
""")

    def setUp(self):
        super(HeaderTestCase, self).setUp()
        self.header = self.main_view.get_header()
        self.label = self.app.select_single(
            'Label', objectName='clicked_label')
        self.assertEqual(self.label.text, 'No button clicked.')

    def test_header_custom_proxy_object(self):
        self.assertIsInstance(self.header, ubuntuuitoolkit.Header)
        self.assertTrue(self.header.visible)
        self.assertEqual(self.header.title, "Test title")

    def test_click_header_action_button(self):
        self.header.click_action_button('action0')
        self.assertEqual(self.label.text, 'Button 0 clicked.')

    def test_click_header_overflow_action_button(self):
        # custom back button and first action button go in the header
        # and the others in the overflow.
        self.header.click_action_button('action3')
        self.assertEqual(self.label.text, 'Button 3 clicked.')

    def test_click_unexisting_header_action_button(self):
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException, self.header.click_action_button,
            'unexisting')
        self.assertEqual(
            str(error),
            'Button not found in header or overflow')

    # back button and tabs button are tested in pagestack and tabs tests
    def test_click_custom_back_button(self):
        self.header.click_custom_back_button()
        self.assertEqual(self.label.text, 'Cancel button clicked.')
