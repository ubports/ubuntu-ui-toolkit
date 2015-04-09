/*
 * Copyright 2015 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import Ubuntu.Components 1.2 as Toolkit12

/*!
  \internal
  documented in PageHeadConfiguration.qdoc
 */
Toolkit12.PageHeadConfiguration {
    id: headerConfig

    property bool locked: false

    // auto-updated by AppHeader, but may be set by the developer
    property bool visible
}
