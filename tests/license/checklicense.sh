#!/bin/bash -e
#
# Copyright 2015 Canonical Ltd.
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

# Based on pbuilderjenkins license checking script.

include_files="\.(c(c|pp|xx)?|h(h|pp|xx)?|p(l|m)|php|py(|x)|java|js|vala|qml)$"
exclude_dirs="(3rd_party|qrc_|moc_|_build)"
allowed_licenses="(Canonical|Android|Google|Digia)"
issues_count=`licensecheck --noconf -r * --copyright -m -c $include_files -i $exclude_dirs | egrep -v $allowed_licenses | wc -l`

if [ $issues_count -eq 0 ]; then
    echo No license problems found.
    exit 0
else
    echo Found $issues_count license problems:
    # Run it a second time to print a nice list of issues
    licensecheck --noconf -r * --copyright -m -c $include_files -i $exclude_dirs | egrep -v $allowed_licenses
    exit 1
fi
