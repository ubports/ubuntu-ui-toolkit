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
 *
 * Author: Andrea Bernabei <andrea.bernabei@canonical.com>
 */

import QtQuick 2.4
import Ubuntu.Components 1.3

/*!
    \qmltype ListItemLayout
    \inqmlmodule Ubuntu.Components 1.3
    \inherits SlotsLayout
    \ingroup ubuntu

    \brief ListItemLayout is a layout component built to be used within
    a \l {ListItem} in a \c ListView.

    ListItemLayout is essentially a \l {SlotsLayout} with a predefined
    \l {SlotsLayout::mainSlot}. This \c mainSlot has been optimized
    to cover most of the usecases without compromising performance.
    The height of this main slot is the minimum height required to
    accomodate the visible and non empty labels it holds.
    If only \l {title} is visible and/or has a text set, for instance,
    the height of the main slot will be \c {title.height}.

    ListItemLayout provides an easy way to create list items which follow
    Ubuntu design standards, thus making them visually appealing and
    consistent with the rest of the platform without effort.

    This layout component works similarly to QtQuick's Row, but while
    Row just positions its children in a horizontal formation,
    ListItemLayout also tweaks their vertical position to ensure
    a clean layout.

    Creating a layout is as easy as putting a few Items inside
    the ListItemLayout. We will call these items "slots".
    All slots will automatically be positioned using the correct
    margins spacing, following the visual rules specified by the
    Ubuntu Design team.
    It is possible to tweak the position of each slot by modifying
    its attached properties.
*/
SlotsLayout {
    /*!
      \qmlproperty Text title

      This property defines the title label and its properties.
      Styling and font properties can be set just like on any other
      Text component, as shown in the following example:

      \qml
      Item {
        ListItemLayout {
            title.text: "Hello"
            title.color: "yellow"
        }
      }
      \endqml
      */
    property alias title: captions.title

    /*!
      \qmlproperty Text subtitle

      This property defines the subtitle label and its properties.
      Styling and font properties can be set by using the prefix
      \c {subtitle.} in a similar way as shown in \l {title}.

      The subtitle aligns
    */
    property alias subtitle: captions.subtitle

    /*!
      \qmlproperty Text summary

      This property defines the subtitle label and its properties.
      Styling and font properties can be set by using the prefix
      \c {summary.} in a similar way as shown in \l {title}.
    */
    property alias summary: captions.summary

    mainSlot: ThreeLabelsSlot {
        id: captions
    }
}
