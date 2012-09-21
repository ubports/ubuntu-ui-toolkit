# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

"""Tavastia autopilot tests."""

from os import remove
import os.path
from tempfile import mktemp

from autopilot.introspection.qt import QtIntrospectionTestMixin
from autopilot.testcase import AutopilotTestCase


def get_module_include_path():
    return os.path.abspath(
        os.path.join(
            os.path.dirname(__file__),
            '..',
            '..',
            '..',
            'modules')
        )


class TavastiaTestCase(AutopilotTestCase, QtIntrospectionTestMixin):

    """A common test case class that provides several useful methods for SDK tests."""

    def setUp(self):
        super(TavastiaTestCase, self).setUp()
        self.launch_test_qml()

    def launch_test_qml(self):
        # If the test class has defined a 'test_qml' class attribute then we
        # write it to disk and launch it inside the Qml Viewer. If not, then we
        # silently do nothing (presumably the test has something else planned).
        if hasattr(self, 'test_qml') and isinstance(self.test_qml, basestring):
            qml_path = mktemp(suffix='.qml')
            open(qml_path, 'w').write(self.test_qml)
            self.addCleanup(remove, qml_path)

            self.app = self.launch_test_application(
                "qmlviewer",
                "-opengl",
                "-I", get_module_include_path(),
                qml_path)
