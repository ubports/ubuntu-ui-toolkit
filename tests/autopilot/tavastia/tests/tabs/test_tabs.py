# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

"""Tests for the Tabs Qml component."""

from autopilot.matchers import Eventually
from textwrap import dedent
from testtools.matchers import Is, Not, Equals
from testtools import skip

from tavastia.tests import TavastiaTestCase

class TabsTests(TavastiaTestCase):
    """Tests for Tabs component."""

    test_qml = dedent("""\
    import QtQuick 2.0
    import Ubuntu.Components 0.1

    Tabs {
       id: tabs
       Tab {
           id: tab1
           title: "Tab 1"
           page: Rectangle {
              anchors.fill: parent 
              color: "#eeeeee"
           }
       }

       Tab {
           id: tab2
           title: "Tab 2"
           page: Rectangle {
              anchors.fill: parent  
              color: "#eeeeee"
           }
       }
    }
    """)

    def test_can_select_tabs(self):
        """Must be able to select the Qml Tabs component."""

        obj = self.app.select_single('Tabs')
        self.assertThat(obj, Not(Is(None)))
    

