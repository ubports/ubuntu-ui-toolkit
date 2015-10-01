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

#include "private/qqmldata_p.h"
#include <QtQml/QQmlEngine>

#include "privates/threelabelsslot_p.h"
#include "uclistitemlayout.h"
#include "uclabel.h"

/*!
    \qmltype ListItemLayout
    \instantiates UCListItemLayout
    \inqmlmodule Ubuntu.Components 1.3
    \inherits SlotsLayout
    \ingroup ubuntu

    \brief ListItemLayout provides an easy way to create list items which follow
    Ubuntu design standards, thus making them visually appealing and
    consistent with the rest of the platform without effort.

    ListItemLayout is essentially a \l {SlotsLayout} with a predefined
    \l {SlotsLayout::mainSlot} that provides (up to) 3 Labels automatically laid out
    according to our UI guidelines.
    This main slot has been optimized to cover most of the usecases
    without compromising performance (read more in \l {Optimizing memory consumption}).

    We're aware there could be usecases which the \l SlotsLayout::mainSlot provided by
    ListItemLayout doesn't cover. If that is the case, please use \l SlotsLayout
    instead of \l ListItemLayout and provide your own \l SlotsLayout::mainSlot.

    ListItemLayout works similarly to QtQuick's Row, but while
    Row just positions its children in a horizontal formation,
    ListItemLayout also tweaks their vertical position to ensure
    a clean layout.

    We will call ListItemLayout's visual children "slots".
    ListItemLayout positions its slots automatically, following the visual rules
    specified by the Ubuntu Design team.
    Because we think flexibility is an important value of our UI components,
    we made it possible to tweak the position of each slot by modifying
    its attached properties (see \l {Advanced layout tweaks}).

    If you need a progression symbol in your list item,
    just add \l ProgressionSlot as a child of your ListItemLayout.
    No manual positioning is needed, the layout will handle it for you.

    To read more about advanced slots positioning or how to handle
    input (mouse or touch) in ListItemLayout, see \l SlotsLayout
    documentation.

    If you don't need the features provided by \l {ListItem} (such as
    the swiping actions),
    you can also use \l ListItemLayout directly as root of your list view delegate
    or inside a MouseArea, as explained in \l {Input handling}.

    The following code example shows how easy it is to create
    even non trivial list items using
    \l ListItem and ListItemLayout:
    \qml
    ListItem {
        height: layout.height + divider.height
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
    times where you might want to tweak the position of one or more slots.
    The following example shows one way to implement a list
    item with a trailing slot holding two labels. What is special about
    this example is that we want the baseline of one label inside the
    trailing slot to align to \l {title}'s baseline and the baseline of
    the other label to align to \l {subtitle}'s baseline.

    \qml
    ListItem {
        id: listItem
        height: layout.height + divider.height

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

    \section1 Labels layout

    The labels in ListItemLayout's default \l SlotsLayout::mainSlot are laid out in a column.
    The \l title is positioned at the top, followed by \l subtitle and \l summary,
    respectively.

    The \l subtitle has its top anchored to \l {title}'s baseline, with a margin of
    4 DPs.

    The \l summary has its top tightly anchored to \l {subtitle}'s bottom.

    The height of the default \l SlotsLayout::mainSlot provided with ListItemLayout
    is the minimum height required to accomodate the \b visible and \b {non-empty}
    labels it holds. If only \l {title} is visible and has a non-empty text set, for instance,
    the height of the main slot will be \c {title.height}.

    If you wish to have the layout process accomodate a label which doesn't have a
    defined text yet, you should set its text property to " ", as shown in the
    following example:
    \qml
    ListItemLayout {
        title.text: "Hello developers!"
        //NOTE: the whitespace
        subtitle.text: " "
    }
    \endqml

    That will make sure \l SlotsLayout::mainSlot is resized to accomodate the (currently empty)
    subtitle.

    This is useful, for instance, if you want all list items in a list view
    to have the same height even without having to fill \l subtitle's (or summary's)
    text with dummy content.

    \section1 Optimizing memory consumption

    In order to minimize memory consumption, the Labels in the \l SlotsLayout::mainSlot
    are only allocated in memory on demand, i.e. the first time one of their
    properties is queried.
    \qml
    ListItemLayout {
        //NOTE: querying subtitle.text triggers allocation of subtitle Label
        Component.onCompleted: console.log(subtitle.text)

        title.text: "Hello developers!"
    }
    \endqml
    In the example above, querying subtitle.text will trigger the
    allocation in memory of the subtitle Label, which we don't actually use.
    We \b recommend avoiding querying properties of labels that we
    don't plan to use in the layout, in order to minimize memory consumption
    and maximize the scrolling performance of our list views.
    \qml
    ListItemLayout {
        //no extra labels created
        title.text: "Hello developers!"
    }
    \endqml

*/
UCListItemLayout::UCListItemLayout(QQuickItem *parent)
    : UCSlotsLayout(parent)
{
    //don't set the parent, we have to create qqmldata first
    UCThreeLabelsSlot *main = new UCThreeLabelsSlot();

    //create QML data for mainSlot otherwise qmlAttachedProperties
    //calls in SlotsLayout will fail
    QQmlData::get(main, true);
    main->setParent(this);

    //this will also set the parentItem
    UCSlotsLayout::setMainSlot(main);
}

/*!
    \qmlproperty Label ListItemLayout::title

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
UCLabel *UCListItemLayout::title()
{
    return qobject_cast<UCThreeLabelsSlot *>(mainSlot())->title();
}

/*!
    \qmlproperty Label ListItemLayout::subtitle

    This property defines the subtitle label and its properties.
    Styling and font properties can be set by using the prefix
    \c {subtitle.} in a similar way as shown in \l title.
*/
UCLabel *UCListItemLayout::subtitle()
{
    return qobject_cast<UCThreeLabelsSlot *>(mainSlot())->subtitle();
}

/*!
    \qmlproperty Label ListItemLayout::summary

    This property defines the subtitle label and its properties.
    Styling and font properties can be set by using the prefix
    \c {summary.} in a similar way as shown in \l title.
*/
UCLabel *UCListItemLayout::summary()
{
    return qobject_cast<UCThreeLabelsSlot *>(mainSlot())->summary();
}

void UCListItemLayout::setMainSlot(QQuickItem *slot) {
    Q_UNUSED(slot);
    qmlInfo(this) << "Setting a different mainSlot on ListItemLayout is not supported. Please use SlotsLayout instead.";
}
