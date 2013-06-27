# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License version 3, as published
# by the Free Software Foundation.

"""Tests for the Ubuntu UI Toolkit Gallery"""

from autopilot.matchers import Eventually
from textwrap import dedent
from testtools.matchers import Is, Not, Equals
from testtools import skip
import os
import time
from UbuntuUiToolkit.tests import UbuntuUiToolkitTestCase


class GenericTests(UbuntuUiToolkitTestCase):
    """Generic tests for the Gallery"""

    # Support both running from system and in the source directory
    runPath = os.path.dirname(os.path.realpath(__file__))
    localSourceFile = runPath + "/../../../../../examples/ubuntu-ui-toolkit-gallery/ubuntu-ui-toolkit-gallery.qml"
    if (os.path.isfile(localSourceFile)):
        print "Using local source directory"
        test_qml_file = localSourceFile
    else:
        print "Using system QML file"
        test_qml_file = "/usr/lib/ubuntu-ui-toolkit/examples/ubuntu-ui-toolkit-gallery/ubuntu-ui-toolkit-gallery.qml"

    def test_0_can_select_mainwindow(self):
        """Must be able to select the main window."""

        rootItem = self.main_view
        self.assertThat(rootItem, Not(Is(None)))
        self.assertThat(rootItem.visible,Eventually(Equals(True)))

    def test_can_select_listview(self):
        """Must be able to select the listview from main"""    

        contentLoader,listView = self.getWidgetLoaderAndListView();

        # Don't have the first, already selected item as the first item to check
        items = [
                    "Navigation",
                    "Toggles", 
                    "Buttons", 
                    "Slider", 
                    "Text Field", 
                    "Progress and activity", 
                    "Ubuntu Shape", 
                    "Icons", 
                    "Label",
                    "List Items", 
                ]                  


        for item in items:
            self.checkListItem(item)
            self.loadItem(item)
            self.checkPageHeader(item)

        # scroll view to expose more items
        self.drag("Icons","Text Field")

        # Wait for the scrolling to finish, the next click fails on the 
        # slower Intel machine but succeeds on AMD and NVIDIA.
        # (LP: #1180226)
        time.sleep(1)

        # now that we have more items, lets continue
        items = [
                    "Dialog", 
                    "Popover",
                    "Sheet",
                    "Animations"
                ]

        for item in items:
            self.checkListItem(item)
            self.loadItem(item)
            self.checkPageHeader(item)

    def test_navigation(self):
        item = "Navigation"
        self.loadItem(item)
        self.checkPageHeader(item)

    def test_toggles(self):
        item = "Toggles"
        self.checkListItem(item)
        self.loadItem(item)
        self.checkPageHeader(item)

        # check default states
        item_data = [
            ["checkbox_unchecked",False,True],
            ["checkbox_checked",True,True],
            ["checkbox_disabled_unchecked",False,False],
            ["checkbox_disabled_checked",True,False],
            ["switch_unchecked",False,True],
            ["switch_checked",True,True],
            ["switch_disabled_unchecked",False,False],
            ["switch_disabled_checked",True,False]
        ]

        for data in item_data:
            objName = data[0]
            objChecked = data[1]
            objEnabled = data[2]

            obj = self.getObject(objName)
            self.assertThat(obj.checked, Equals(objChecked))
            self.assertThat(obj.enabled, Equals(objEnabled))
            
            # try to interact with objects
            self.tap(objName)

            if (obj.enabled):
                self.assertThat(obj.checked, Not(Equals(objChecked)))
            else:
                self.assertThat(obj.checked, Equals(objChecked))

    def test_buttons(self):
        item = "Buttons"
        self.loadItem(item)
        self.checkPageHeader(item)

        item_data = [
            ["button_text",True,None,None,"Call"],
            ["button_text_disabled",False,None,None,"Call"],
            ["button_color",True,[93,163,87,255],None,"Call"],
            ["button_iconsource",True,None,"call_icon.png",None],
            ["button_iconsource_text",True,None,"call_icon.png","Call"]
        ]

        for data in item_data:
            objName = data[0]
            objEnabled = data[1]
            objColor = data[2]
            objIcon = data[3]
            objText = data[4]

            obj = self.getObject(objName)
            self.assertThat(obj.enabled, Equals(objEnabled))

            print obj.color
            if (objColor != None):
                self.assertThat(obj.color, Equals(objColor))

            if (objIcon != None):
                self.assertThat(obj.iconSource.endswith(objIcon), Equals(True))

            if (objText != None):
                self.assertThat(obj.text, Equals(objText))
            
            # try to interact with objects
            self.mousePress(objName)

            if (obj.enabled):
                self.assertThat(obj.pressed, Equals(True))
            else:
                self.assertThat(obj.pressed, Equals(False))

            self.mouseRelease()

            self.assertThat(obj.pressed, Equals(False))
    
    def test_slider(self):
        item = "Slider"
        self.loadItem(item)
        self.checkPageHeader(item)

        item_data = [
            [ "slider_standard" ],
            [ "slider_live" ],
            [ "slider_range" ]
        ]

        for data in item_data:
            objName = data[0]
            obj = self.getObject(objName)
            self.tap(objName)

            # TODO: move slider value
        
#     def test_textarea(self):
#         item = "Text Field"
#         self.loadItem(item)
#         self.checkPageHeader(item)

#         template_textinputs = self.getObject("textinputs")
        
#         item_data = [
#             [ "textarea_default", True, -1, template_textinputs.longText, None ],
#             [ "textarea_expanding", True, -1, "", None],
#             [ "textarea_richtext", True, -1, template_textinputs.richText, None ]
#         ]

#         for data in item_data:
#             objName = data[0]
#             objEnabled = data[1]
#             objEchoMode = data[2]
#             objText = data[3]
#             objNumbersOnly = data[4]

#             obj = self.getObject(objName)
#             self.tap(objName)

#             self.assertThat(obj.enabled, Equals(objEnabled))
# #            self.assertThat(obj.focus, Equals(obj.enabled)) 
#             self.assertThat(obj.highlighted, Equals(obj.focus))
#             #self.assertThat(obj.hasClearButton, Equals(True))
#             self.assertThat(obj.text, Equals(objText))

            #TODO: There is no clear button or method to clear a textarea?
            #self.tap_clearButton(objName)

            # self.assertThat(obj.text,Equals(""))

            # self.type_string("Hello World!")

            # self.assertThat(obj.text,Equals("Hello World!"))




    def test_textfield(self):
        item = "Text Field"
        self.loadItem(item)
        self.checkPageHeader(item)

        template_textinputs = self.getObject("textinputs")
        
        item_data = [
            [ "textfield_standard", True, 0, "", None ],
            [ "textfield_password", True, 2, "password", None ],
            [ "textfield_numbers", True, 0, "123", True ],
            [ "textfield_disabled", False, 0, "", None ],
        ]

        for data in item_data:
            objName = data[0]
            objEnabled = data[1]
            objEchoMode = data[2]
            objText = data[3]
            objNumbersOnly = data[4]

            obj = self.getObject(objName)
            self.tap(objName)

            self.assertThat(obj.enabled, Equals(objEnabled))
            self.assertThat(obj.focus, Equals(obj.enabled)) 
            self.assertThat(obj.highlighted, Equals(obj.focus))
            self.assertThat(obj.errorHighlight, Equals(False))
            self.assertThat(obj.acceptableInput, Equals(True))
            self.assertThat(obj.hasClearButton, Equals(True))
            self.assertThat(obj.text, Equals(objText))

            if (objEchoMode!=-1):
                self.assertThat(obj.echoMode, Equals(objEchoMode))
            
            if (objNumbersOnly):
                self.type_string("abc")
                self.assertThat(obj.text,Equals(objText))
                self.assertThat(obj.errorHighlight, Equals(False))
                self.assertThat(obj.acceptableInput, Equals(True))
            else:
                self.type_string("Hello World!")
                if (objEnabled):
                    self.assertThat(obj.text, Equals("%sHello World!" % (objText)))
                    self.assertThat(obj.errorHighlight, Equals(False))
                    self.assertThat(obj.acceptableInput, Equals(True))
                else:
                    self.assertThat(obj.text, Equals(objText))

            self.tap_clearButton(objName)


            

    def test_progress_and_activity(self):
        item = "Progress and activity"
        self.loadItem(item)
        self.checkPageHeader(item)

        item_data = [
            [ "progressbar_standard" ],
            [ "progressbar_indeterminate" ],
            [ "activityindicator_standard" ]
        ]

        for data in item_data:
            objName = data[0]
            obj = self.getObject(objName)
            self.tap(objName)

            # TODO: check for properties
        

    def test_ubuntushape(self):
        item = "Ubuntu Shape"
        self.loadItem(item)
        self.checkPageHeader(item)

        item_data = [
            [ "ubuntushape_color_hex" ],
            [ "ubuntushape_color_lightblue" ],
            [ "ubuntushape_color_darkgray" ],
            [ "ubuntushape_image" ],
            [ "ubuntushape_radius_small" ],
            [ "ubuntushape_radius_medium" ],
            [ "ubuntushape_sizes_15_6" ],
            [ "ubuntushape_sizes_10_14" ]
        ]

        for data in item_data:
            objName = data[0]
            obj = self.getObject(objName)
        
        
