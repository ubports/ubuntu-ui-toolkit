# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2013, 2014, 2015 Canonical Ltd.
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

import os

from ubuntuuitoolkit import tests


class DialogTestCase(tests.QMLFileAppTestCase):
    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    test_qml_file_path = os.path.join(
        dir_path, 'test_dialog.DialogTestCase.qml')

    def test_dialog(self):
        open_button = self.main_view.wait_select_single('Button',
                                                        objectName='button')
        self.pointing_device.click_object(open_button)
        dialog = self.main_view.wait_select_single('Dialog',
                                                   title="Dialog")
        text_area = dialog.wait_select_single('TextArea',
                                              objectName='textfield_standard')
        text_area.write('test')
        self.assertEqual(text_area.text, 'test')
        lower_button = dialog.wait_select_single('Button',
                                                 text='Lower button')
        self.pointing_device.click_object(lower_button)
