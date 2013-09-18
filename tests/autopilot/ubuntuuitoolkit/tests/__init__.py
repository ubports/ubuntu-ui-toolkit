# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2012, 2013 Canonical Ltd.
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

from os import remove
import os.path
import tempfile
import subprocess

from autopilot.input import Pointer
from autopilot.matchers import Eventually
from testtools.matchers import Is, Not, Equals

from ubuntuuitoolkit import base, emulators


def get_module_include_path():
    return os.path.abspath(
        os.path.join(
            os.path.dirname(__file__),
            '..',
            '..',
            '..',
            '..',
            'modules')
    )


def get_qmlscene_path():
    arch = subprocess.check_output(
        ['dpkg-architecture', '-qDEB_HOST_MULTIARCH']).strip()
    return '/usr/lib/{0}/qt5/bin/qmlscene'.format(arch)


class QMLStringAppTestCase(base.UbuntuUIToolkitAppTestCase):
    """Base test case for self tests that define the QML on an string."""

    desktop_file_contents = ("""
[Desktop Entry]
Type=Application
Exec=Not important
Path=Not important
Name=Test app
Icon=Not important
""")

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
        qml_file_path = self._write_test_qml_file()
        desktop_file_path = self._write_desktop_file()
        self.app = self.launch_test_application(
            get_qmlscene_path(),
            '-I' + get_module_include_path(),
            qml_file_path,
            'desktop_file_hint={0}'.format(desktop_file_path),
            emulator_base=emulators.UbuntuUIToolkitEmulatorBase,
            app_type='qt')

        self.assertThat(
            self.main_view.visible, Eventually(Equals(True)))

    def _write_test_qml_file(self):
        qml_file_path = tempfile.mkstemp(suffix='.qml')
        open(qml_file_path, 'w').write(self.test_qml)
        self.addCleanup(remove, qml_file_path)
        return qml_file_path

    def _write_desktop_file(self):
        desktop_file_dir = os.path.join(
            os.environ['HOME'], '.local', 'share', 'applications')
        desktop_file_path = tempfile.mkstemp(
            suffix='.desktop', dir=desktop_file_dir)
        open(desktop_file_path, 'w').write(self.desktop_file_contents)
        self.addCleanup(remove, desktop_file_path)
        return desktop_file_path

    @property
    def main_view(self):
        return self.app.select_single(emulators.MainView)


class QMLFileAppTestCase(base.UbuntuUIToolkitAppTestCase):
    """Base test case for self tests that launch a QML file."""

    test_qml_file_path = '/path/to/file.qml'

    def setUp(self):
        super(QMLFileAppTestCase, self).setUp()
        self.pointing_device = Pointer(self.input_device_class.create())
        self.launch_application()

    def launch_application(self):
        self.app = self.launch_test_application(
            get_qmlscene_path(),
            "-I" + get_module_include_path(),
            self.test_qml_file_path,
            emulator_base=emulators.UbuntuUIToolkitEmulatorBase,
            app_type='qt')

        self.assertThat(
            self.main_view.visible, Eventually(Equals(True)))

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
        self.assertThat(textField.hasClearButton, Equals(True))
        btn = textField.select_single("AbstractButton")
        self.pointing_device.move_to_object(btn)
        self.pointing_device.click()
