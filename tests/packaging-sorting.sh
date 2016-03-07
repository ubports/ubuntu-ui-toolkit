#!/bin/bash
# Copyright 2016 Canonical Ltd.
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
# Author: Timo Jyrinki <timo.jyrinki@canonical.com>

# Ensure packaging has gone through wrap-and-sort command

if [ ! -f  "/usr/bin/wrap-and-sort" ] ; then
  echo "Please install 'devscripts' package"
  exit 1
fi

cd $(dirname $0)
tmpdir=$(mktemp -d)
cp -a ../debian $tmpdir

wrap-and-sort -a -t -d $tmpdir/debian/

diff -urN ../debian $tmpdir/debian

if [ $? == 1 ] ; then
 echo 
 echo 
 echo "*******************************************************"
 echo "Please run 'wrap-and-sort -a -t' to clean up packaging."
 echo "*******************************************************"
 echo 
 exit 1
fi

