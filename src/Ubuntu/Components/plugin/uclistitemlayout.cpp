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

#include "uclistitemlayout.h"
#include "private/qqmldata_p.h"
#include <QtQml/QQmlEngine>

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
    spacing, following the visual rules specified by the
    Ubuntu Design team.
    It is possible to tweak the position of each slot by modifying
    its attached properties.

    The progression symbol to be used in list items which handle
    navigation in a page stack is provided by \l ProgressionSlot.

    To read more about advanced slots positioning or how to handle
    input (mouse or touch) in ListItemLayout, see \l SlotsLayout
    documentation.

    If you don't need the swiping actions provided by \l {ListItem},
    you can also use \l ListItemLayout directly inside a MouseArea
    as delegate for your list view, as explained in \l SlotsLayout
    documentation.

    The following code example shows how easy it is to create
    even non trivial list items list items using
    \l ListItem and ListItemLayout:
    \qml
    ListItem {
        height: layout.height
        ListItemLayout {
            id: layout
            title.text: "Hello developers!"
            subtitle.text: "I'm a subtitle, I'm tiny!"
            summary.text: "Ubuntu!"

            CheckBox { SlotsLayout.position: SlotsLayout.Leading }

            Icon {
                name: "message"
                SlotsLayout.position: SlotsLayout.Trailing;
                width: units.gu(2)
            }
        }
    }
    \endqml

    Although ListItemLayout covers most of the usecases, there might be
    times where manually tweaking the positioning is needed in order to
    implement list item which need a special layout for one or more of
    its slots. The following example shows one way to implement a list
    item with a trailing slot holding two labels. What is special about
    this example is that we want the baseline of one label inside the
    trailing slot to align to \l {title}'s baseline and the baseline of
    the other label to align to \l {subtitle}'s baseline.

    \qml
        ListItem {
            id: listItem
            height: layout.height

            ListItemLayout {
                id: layout
                title.text: "Hello..."
                title.color: UbuntuColors.Orange
                subtitle.text: "...world!"

                Rectangle {
                    SlotsLayout.position: SlotsLayout.Leading
                    color: "pink"
                    height: units.gu(6)
                    width: height
                }

                Item {
                    id: slot
                    width: secondLabel.width
                    height: parent.height

                    //as we want to position labels to align with title and subtitle
                    SlotsLayout.overrideVerticalPositioning: true

                    Label {
                        id: firstLabel
                        anchors.right: secondLabel.right
                        text: "19:17"
                        fontSize: "small"
                        y: layout.mainSlot.y + layout.title.y
                           + layout.title.baselineOffset - baselineOffset
                    }

                    Label {
                        id: secondLabel
                        text: "Outgoing"
                        fontSize: "small"
                        y: layout.mainSlot.y + layout.subtitle.y
                           + layout.subtitle.baselineOffset - baselineOffset
                    }
                }
            }
        }
    \endqml
    \sa SlotsLayout
    \sa ProgressionSlot
*/
UCListItemLayout::UCListItemLayout(QQuickItem *parent)
    : UCSlotsLayout(parent)
{
    //don't set the parent, we have to create qqmldata first
    UCThreeLabelsSlot *main = new UCThreeLabelsSlot();

    //create QML data for mainSlot otherwise qmlAttachedProperties
    //calls in SlotsLayout will fail
    QQmlData::get(main, true);

    //this will also set the parent
    setMainSlot(main);
}

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
QQuickText *UCListItemLayout::title()
{
    return qobject_cast<UCThreeLabelsSlot *>(mainSlot())->title();
}

/*!
  \qmlproperty Text subtitle

  This property defines the subtitle label and its properties.
  Styling and font properties can be set by using the prefix
  \c {subtitle.} in a similar way as shown in \l {title}.
*/
QQuickText *UCListItemLayout::subtitle()
{
    return qobject_cast<UCThreeLabelsSlot *>(mainSlot())->subtitle();
}

/*!
  \qmlproperty Text summary

  This property defines the subtitle label and its properties.
  Styling and font properties can be set by using the prefix
  \c {summary.} in a similar way as shown in \l {title}.
*/
QQuickText *UCListItemLayout::summary()
{
    return qobject_cast<UCThreeLabelsSlot *>(mainSlot())->summary();
}
