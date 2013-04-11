#!/bin/bash
#
# Copyright 2013 Canonical Ltd.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; version 3.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
################################################################################
issuescount=`licensecheck --noconf -r * --copyright -m -c "\.(c(c|pp|xx)?|h(h|pp|xx)?|p(l|m)|php|py(|x)|java|js|vala|qml)$"|egrep -v "(Canonical|GENERATED FILE)"|wc -l`
if [ $issuescount -eq 0 ]; then
    echo No license problems found.
    exit 0
else
    echo Found $issuescount license problems:
    licensecheck --noconf -r * --copyright -m -c "\.(c(c|pp|xx)?|h(h|pp|xx)?|p(l|m)|php|py(|x)|java|js|vala|qml)$"|egrep -v "(Canonical|GENERATED FILE)"
    exit 1
fi
