# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.


from UbuntuUiToolkit import tests
from UbuntuUiToolkit import emulators


class MainViewTestCase(tests.UbuntuUiToolkitTestCase):

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

    def test_main_view_custom_emulator(self):
        self.assertIsInstance(self.main_view, emulators.MainView)

    def test_get_header(self):
        header = self.main_view.header
        self.assertEqual(header.title, "Test title")
