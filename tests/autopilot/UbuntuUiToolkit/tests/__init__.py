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
from tempfile import mktemp
import subprocess

from autopilot.input import Mouse, Touch, Pointer
from autopilot.matchers import Eventually
from autopilot.platform import model
from testtools.matchers import Is, Not, Equals
from autopilot.testcase import AutopilotTestCase

from UbuntuUiToolkit.emulators.main_window import MainWindow


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


class UbuntuUiToolkitTestCase(AutopilotTestCase):

    """Common test case class for SDK tests."""

    if model() == 'Desktop':
        scenarios = [
            ('with mouse', dict(input_device_class=Mouse))
        ]
    else:
        scenarios = [
            ('with touch', dict(input_device_class=Touch))
        ]

    def setUp(self):
        self.pointing_device = Pointer(self.input_device_class.create())
        super(UbuntuUiToolkitTestCase, self).setUp()
        self.launch_test_qml()

    def launch_test_qml(self):
        # If the test class has defined a 'test_qml' class attribute then we
        # write it to disk and launch it inside the Qml Viewer. If not, then we
        # silently do nothing (presumably the test has something else planned).
        arch = subprocess.check_output(
            ["dpkg-architecture", "-qDEB_HOST_MULTIARCH"]).strip()
        if hasattr(self, 'test_qml') and isinstance(self.test_qml, basestring):
            qml_path = mktemp(suffix='.qml')
            open(qml_path, 'w').write(self.test_qml)
            self.addCleanup(remove, qml_path)

            self.app = self.launch_test_application(
                "/usr/lib/" + arch + "/qt5/bin/qmlscene",
                "-I", get_module_include_path(),
                qml_path)

        if (hasattr(self, 'test_qml_file') and
                isinstance(self.test_qml_file, basestring)):
            qml_path = self.test_qml_file
            self.app = self.launch_test_application(
                "/usr/lib/" + arch + "/qt5/bin/qmlscene",
                "-I", get_module_include_path(),
                qml_path)

        self.assertThat(
            self.main_window.get_qml_view().visible, Eventually(Equals(True)))

    def checkListItem(self, itemText):
        item = self.main_window.get_object_by_text("Standard", itemText)
        self.assertThat(item, Not(Is(None)))

    def getListItem(self, itemText):
        return self.main_window.get_object_by_text("Standard", itemText)

    def getWidgetLoaderAndListView(self):
        contentLoader = self.main_window.get_object(
            "QQuickLoader", "contentLoader")
        listView = self.main_window.get_object("QQuickListView", "widgetList")
        self.assertThat(listView, Not(Is(None)))
        self.assertThat(listView.visible, Eventually(Equals(True)))
        return (contentLoader, listView)

    def loadItem(self, item):
        contentLoader = self.main_window.get_object(
            "QQuickLoader", "contentLoader")
        self.selectItem(item)
        self.assertThat(contentLoader.progress, Eventually(Equals(1.0)))
        loadedPage = self.main_window.get_object_by_text("Standard", item)
        self.assertThat(loadedPage, Not(Is(None)))
        self.assertThat(loadedPage.visible, Eventually(Equals(True)))

    def drag(self, itemText, itemTextTo):
        item = self.getListItem(itemText)
        itemTo = self.getListItem(itemTextTo)

        self.pointing_device.move_to_object(item)
        self.pointing_device.press()
        self.pointing_device.move_to_object(itemTo)
        self.pointing_device.release()

    def selectItem(self, itemText):
        item = self.getListItem(itemText)
        self.assertThat(item.selected, Eventually(Equals(False)))

        self.pointing_device.move_to_object(item)
        self.pointing_device.click()

        self.assertThat(item.selected, Eventually(Equals(True)))

    def getMainView(self):
        mainView = self.app.select_many("MainView")[0]
        self.assertThat(mainView, Not(Is(None)))
        return mainView

    def getOrientationHelper(self):
        orientationHelper = self.getMainView().select_many(
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

    @property
    def main_window(self):
        return MainWindow(self.app)
