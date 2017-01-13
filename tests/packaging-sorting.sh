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

# Get the current script directory (compatible with Bash and ZSH)
SCRIPT_DIR=`dirname ${BASH_SOURCE[0]-$0}`
SCRIPT_DIR=`cd $SCRIPT_DIR && pwd`

SRC=$SCRIPT_DIR/..
# Ensure packaging has gone through wrap-and-sort command
tmpdir=$(mktemp -d)
cp -a $SRC/debian $tmpdir
wrap-and-sort -a -t -d $tmpdir/debian/
[ $? == 2 ] && exit 2
# Note: control.gles may be moved in gles builds
if [ -f $tmpdir/debian/control.gles ] ; then
 # Verify control.gles which otherwise isn't picked up
 wrap-and-sort -a -t -d $tmpdir/debian/ -f $tmpdir/debian/control.gles
fi
[ $? == 2 ] && exit 2

diff -urN $SRC/debian $tmpdir/debian

if [ $? == 1 ] ; then
 echo 
 echo 
 echo "*******************************************************"
 echo "Please run 'wrap-and-sort -a -t; wrap-and-sort -a -t -f debian/control.gles' to clean up packaging."
 echo "*******************************************************"
 echo 
 exit 1
fi

