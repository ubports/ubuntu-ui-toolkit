# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

"""Tests for the QML PageStack component."""

from autopilot.matchers import Eventually
from textwrap import dedent
from testtools.matchers import Is, Not, Equals
from testtools import skip
import os
from tavastia.tests import TavastiaTestCase

class PageStackTests(TavastiaTestCase):
    """Tests for PageStack component."""

    test_qml = dedent("""\
import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

PageStack {
    id: pageStack

    width: 300
    height: 400

    Component.onCompleted: push(page0)

    Page {
        id: page0
        title: "Root page"

        Column {
            anchors.fill: parent
            ListItem.Standard {
                text: "Page one"
                onClicked: pageStack.push(page1)
                progression: true
            }
        }
    }

    Page {
        id: page1
        title: "First page"

        Column {
            anchors.fill: parent
          
            ListItem.Standard {
                text: "Red rectangle"
                onClicked: pageStack.push(rect, {color: "red"})
                progression: true
            }
        }
    }

    Component {
        id: rect
        Rectangle {
            id: rectangle
            anchors.fill: parent
            color: "green"
        }
    }
}
    """)

    def test_can_select_pagestack(self):
        """Must be able to select the Qml PageStack component."""

        obj = self.app.select_single('PageStack')
        self.assertThat(obj, Not(Is(None)))
    

