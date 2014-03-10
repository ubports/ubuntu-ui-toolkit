# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2014 Canonical Ltd.
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

import testtools

from ubuntuuitoolkit import environment


class InitctlEnvironmentVariableTestCase(testtools.TestCase):

    def test_is_environment_variable_set_with_unset_variable(self):
        variable = 'I do not exist'
        self.assertFalse(environment.is_initctl_env_var_set(variable))

    def test_is_environment_variable_set_with_set_variable(self):
        variable = 'Testvariabletoset'
        self.addCleanup(environment.unset_initctl_env_var, variable)

        environment.set_initctl_env_var(variable, 'dummy')

        self.assertTrue(environment.is_initctl_env_var_set(variable))

    def test_get_environment_variable(self):
        variable = 'Testvariabletoget'
        self.addCleanup(environment.unset_initctl_env_var, variable)
        environment.set_initctl_env_var(variable, 'test value')

        self.assertEqual(
            'test value', environment.get_initctl_env_var(variable))

    def test_unset_environment_variable(self):
        variable = 'Testvariabletoget'
        environment.set_initctl_env_var(variable, 'dummy')

        environment.unset_initctl_env_var(variable)

        self.assertFalse(environment.is_initctl_env_var_set(variable))

    def test_unset_environment_variable_with_unset_variable(self):
        variable = 'I do not exist'

        environment.unset_initctl_env_var(variable)

        self.assertFalse(environment.is_initctl_env_var_set(variable))
