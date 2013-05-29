#!/bin/sh
#
# Copyright 2012 Canonical Ltd.
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

sed -r -i 's@("main-content">)@\1<ul class="breadcrumb">@g' $1/html/*.html
sed -r -i 's@(<div class="toc">|<h1 class="title">)@</ul>\1@g' $1/html/*.html
sed -r -i 's@(</p>)<p/>@\1@g' $1/html/*.html
