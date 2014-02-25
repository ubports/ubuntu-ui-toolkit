# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2012, 2013, 2014 Canonical Ltd.
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

"""Ubuntu UI Toolkit autopilot tests."""

import os
import tempfile

from autopilot.input import Pointer
from autopilot.matchers import Eventually
from testtools.matchers import Is, Not, Equals

from ubuntuuitoolkit import base, emulators, fixture_setup


_DESKTOP_FILE_CONTENTS = ("""[Desktop Entry]
Type=Application
Exec=Not important
Path=Not important
Name=Test app
Icon=Not important
""")


def _write_test_desktop_file():
    desktop_file_dir = get_local_desktop_file_directory()
    if not os.path.exists(desktop_file_dir):
        os.makedirs(desktop_file_dir)
    desktop_file = tempfile.NamedTemporaryFile(
        suffix='.desktop', dir=desktop_file_dir, delete=False)
    desktop_file.write(_DESKTOP_FILE_CONTENTS.encode('utf-8'))
    desktop_file.close()
    return desktop_file.name


def get_local_desktop_file_directory():
    return os.path.join(os.environ['HOME'], '.local', 'share', 'applications')


def _get_module_include_path():
    return os.path.join(get_path_to_source_root(), 'modules')


def get_path_to_source_root():
    return os.path.abspath(
        os.path.join(
            os.path.dirname(__file__), '..', '..', '..', '..'))


class QMLStringAppTestCase(base.UbuntuUIToolkitAppTestCase):
    """Base test case for self tests that define the QML on an string."""

    test_qml = ("""
import QtQuick 2.0
import Ubuntu.Components 0.1

MainView {
    width: units.gu(48)
    height: units.gu(60)
}
""")

    def setUp(self):
        super(QMLStringAppTestCase, self).setUp()
        self.pointing_device = Pointer(self.input_device_class.create())
        self.launch_application()

    def launch_application(self):
        fake_application = fixture_setup.FakeApplication(
            qml_file_contents=self.test_qml)
        self.useFixture(fake_application)

        self.app = self.launch_test_application(
            base.get_qmlscene_launch_command(),
            '-I' + _get_module_include_path(),
            fake_application.qml_file_path,
            '--desktop_file_hint={0}'.format(
                fake_application.desktop_file_path),
            emulator_base=emulators.UbuntuUIToolkitEmulatorBase,
            app_type='qt')

        self.assertThat(
            self.main_view.visible, Eventually(Equals(True)))

    @property
    def main_view(self):
        return self.app.select_single(emulators.MainView)


class FlickDirection:
    """Enum for flick or scroll direction."""

    UP, DOWN, LEFT, RIGHT = range(0, 4)


class QMLFileAppTestCase(base.UbuntuUIToolkitAppTestCase):
    """Base test case for self tests that launch a QML file."""

    test_qml_file_path = '/path/to/file.qml'
    desktop_file_path = None

    def setUp(self):
        super(QMLFileAppTestCase, self).setUp()
        self.pointing_device = Pointer(self.input_device_class.create())
        self.launch_application()

    def launch_application(self):
        desktop_file_path = self._get_desktop_file_path()
        self.app = self.launch_test_application(
            base.get_qmlscene_launch_command(),
            "-I" + _get_module_include_path(),
            self.test_qml_file_path,
            '--desktop_file_hint={0}'.format(desktop_file_path),
            emulator_base=emulators.UbuntuUIToolkitEmulatorBase,
            app_type='qt')

        self.assertThat(
            self.main_view.visible, Eventually(Equals(True)))

    def _get_desktop_file_path(self):
        if self.desktop_file_path is None:
            desktop_file_path = _write_test_desktop_file()
            self.addCleanup(os.remove, desktop_file_path)
            return desktop_file_path
        else:
            self.desktop_file_path

    @property
    def main_view(self):
        return self.app.select_single(emulators.MainView)

    def checkListItem(self, itemText):
        item = self.getListItem(itemText)
        self.assertThat(item, Not(Is(None)))

    def getListItem(self, itemText):
        # XXX We shouldn't access the elements by text, because that's likely
        # to change often and might be translated. We should always use the
        # objectName instead. --elopio - 2013-06-26216
        return self.main_view.select_single("Standard", text=itemText)

    def getWidgetLoaderAndListView(self):
        contentLoader = self.main_view.select_single(
            "QQuickLoader", objectName="contentLoader")
        listView = self.main_view.select_single(
            "QQuickListView", objectName="widgetList")
        self.assertThat(listView, Not(Is(None)))
        self.assertThat(listView.visible, Eventually(Equals(True)))
        return (contentLoader, listView)

    def loadItem(self, item):
        self.selectItem(item)
        contentLoader = self.main_view.select_single(
            "QQuickLoader", objectName="contentLoader")
        self.assertThat(contentLoader.progress, Eventually(Equals(1.0)))
        loadedPage = self.getListItem(item)
        self.assertThat(loadedPage, Not(Is(None)))
        #loadedPage is not a page, it is the list item which goes in
        #background when the item is selected, which changes the visible
        #property of item in list itself to False. So followin check
        #fails on Nexus 4. Commenting it for now.
        #self.assertThat(loadedPage.visible, Eventually(Equals(True)))

    def drag(self, itemText, itemTextTo):
        item = self.getListItem(itemText)
        itemTo = self.getListItem(itemTextTo)

        self.pointing_device.move_to_object(item)
        self.pointing_device.press()
        self.pointing_device.move_to_object(itemTo)
        self.pointing_device.release()

    def reveal_item_by_flick(self, item, flickable, direction):
        x1, y1, w1, h1 = item.globalRect
        x2, y2, w2, h2 = flickable.globalRect
        if direction is FlickDirection.UP:
            while y1 + h1 > y2 + h2:
                self.flick(flickable, direction)
                x1, y1, w1, h1 = item.globalRect
        elif direction is FlickDirection.DOWN:
            while y1 < y2:
                self.flick(flickable, direction)
                x1, y1, w1, h1 = item.globalRect

    def flick(self, flickable, direction, delta=40):
        """This funcito flicks the page from middle to the given direction."""
        x, y, w, h = flickable.globalRect
        if direction == FlickDirection.UP:
            self.pointing_device.drag(x + w / 2, y + h / 2, x + w / 2,
                                      y + h / 2 - delta)
            flickable.flicking.wait_for(False)
        elif direction == FlickDirection.DOWN:
            self.pointing_device.drag(x + w / 2, y + h / 2, x + w / 2,
                                      y + h / 2 + delta)
            flickable.flicking.wait_for(False)
        else:
            raise ValueError("Invalid direction or not implementd yet")

    def selectItem(self, itemText):
        item = self.getListItem(itemText)
        x1, y1, w1, h1 = item.globalRect
        x2, y2, w2, h2 = self.main_view.globalRect

        orientationHelper = self.getOrientationHelper()
        rot = orientationHelper.rotation
        scrollTo = h2 / 2 - (y1 - h2 - h1)
        if rot == 0.0 and y1 > h2:
            self.pointing_device.drag(w2 / 2, h2 / 2, w2 / 2, scrollTo)

        self.assertThat(item.selected, Eventually(Equals(False)))

        self.pointing_device.move_to_object(item)
        self.pointing_device.click()

        self.assertThat(item.selected, Eventually(Equals(True)))

    def getOrientationHelper(self):
        orientationHelper = self.main_view.select_many(
            "OrientationHelper")[0]
        self.assertThat(orientationHelper, Not(Is(None)))
        return orientationHelper

    def checkPageHeader(self, pageTitle):
        orientationHelper = self.getOrientationHelper()
        header = orientationHelper.select_many("Header", title=pageTitle)[0]
        self.assertThat(header, Not(Is(None)))
        self.assertThat(header.visible, Eventually(Equals(True)))
        return header

    def getObject(self, objectName):
        obj = self.app.select_single(objectName=objectName)
        self.assertThat(obj, Not(Is(None)))
        return obj

    def tap(self, objectName):
        obj = self.getObject(objectName)
        self.pointing_device.move_to_object(obj)
        self.pointing_device.click()

    def mousePress(self, objectName):
        obj = self.getObject(objectName)
        self.pointing_device.move_to_object(obj)
        self.pointing_device.press()

    def mouseRelease(self):
        self.pointing_device.release()

    def type_string(self, string):
        self.keyboard.type(string)

    def type_key(self, key):
        self.keyboard.key(key)

    def tap_clearButton(self, objectName):
        textField = self.getObject(objectName)
        self.assertIsNotNone(textField)
        self.pointing_device.click_object(textField)
        self.assertThat(textField.focus, Eventually(Equals(True)))
        self.assertThat(textField.hasClearButton, Eventually(Equals(True)))
        btn = textField.select_single("AbstractButton")
        self.assertIsNotNone(btn)
        self.assertThat(btn.visible, Eventually(Equals(True)))
        self.pointing_device.click_object(btn)
        self.assertThat(btn.pressed, Eventually(Equals(False)))
