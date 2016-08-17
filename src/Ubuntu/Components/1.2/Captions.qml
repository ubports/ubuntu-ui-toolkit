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

import QtQuick 2.4
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.2

/*!
  \qmltype Captions
  \inqmlmodule Ubuntu.Components
  \ingroup ubuntu-listitem
  \since Ubuntu.Components 1.2
  \brief Container providing captionStyles for a twin-label column that can be used in
  RowLayout or GridLayout.

  The labels are placed in a column and can be accessed through \l title and
  \l subtitle properties. The default spacing between the labels is 0.5 grid units.

  The container only shows the labels which has its text property set to a valid
  string. The labels not having any text set are not occupying the space. When
  embedded in a positioner or in a RowLayout, the container is aligned vertically
  centered. It is recommended to be used in context with RowLayout or GridLayout,
  however can be used with any component.

  \qml
  import QtQuick 2.4
  import Ubuntu.Components 1.2

  ListItem {
      RowLayout {
           Captions {
               title.text: "Caption"
               subtitle.text: "Subtitle text"
               // attached properties of an Item type property
               // can be changed only with assignment
               Component.onCompleted: subtitle.Layout.alignment = Qt.AlignRight
           }
           Captions {
               captionStyle: Ubuntu.SummaryCaptionStyle
               title.text: "Text"
               subtitle.text: "Text"
           }
      }
  }
  \endqml

  Additional items can also be added to the layout after the two labels.
  \qml
  Captions {
      title.text: "Caption"
      subtitle.text: "Subtitle"
      Label {
          text: "third line"
          fontSize: "xx-small"
      }
  }
  \endqml
  */
ColumnLayout {
    id: captions

    /*!
      \qmlproperty enumeration captionStyle
      The property configures the arrangement and font sizes of the Labels in the
      component. It can take the following values:
      \list
        \li \b Ubuntu.TitleCaptionStyle - (default) typical configuration for a left aligned
            twin-label setup, where the text covers the remaining area on a list
            layout.
        \li \b Ubuntu.SummaryCaptionStyle - configuration for a right-aligned twin label setup,
            with 6 grid units width.
      \endlist
      */
    property int captionStyle: Ubuntu.TitleCaptionStyle

    /*!
      \qmlproperty Label title
      \readonly
      Label occupying the top area of the container.
      */
    property alias title: titleLabel

    /*!
      \qmlproperty Label subtitle
      \readonly
      Label occupying the bottom area of the container.
      */
    property alias subtitle: subtitleLabel

    clip: true
    spacing: units.gu(0.5)
    Layout.fillWidth: (captionStyle == Ubuntu.TitleCaptionStyle)
    Layout.alignment: Qt.AlignVCenter | ((captionStyle == Ubuntu.SummaryCaptionStyle) ? Qt.AlignRight : Qt.AlignLeft)
    Layout.minimumWidth: 0
    Layout.maximumWidth: (captionStyle == Ubuntu.SummaryCaptionStyle) ? units.gu(6) : parent.width
    Layout.preferredWidth: (captionStyle == Ubuntu.SummaryCaptionStyle) ? Layout.maximumWidth : 0
    Layout.minimumHeight: 0
    Layout.maximumHeight: parent ? parent.height : childrenRect.height
    Layout.preferredHeight: childrenRect.height

    // handle visibility, do not override visible property!
    // 'hide' column if title and subtitle is an empty string and
    // there's no more children added
    states: State {
        name: "__q_invisible"
        when: (title.text === "" && subtitle.text === "") && (captions.children.length == 2)
        PropertyChanges {
            target: captions
            visible: false
        }
    }

    Label {
        id: titleLabel
        anchors {
            left: parent.left
            right: parent.right
        }
        fontSize: (captionStyle == Ubuntu.SummaryCaptionStyle) ? "small" : "medium"
        horizontalAlignment: (captionStyle == Ubuntu.SummaryCaptionStyle) ? Text.AlignRight : Text.AlignLeft
        visible: text !== ""
        elide: (captionStyle == Ubuntu.TitleCaptionStyle) ? Text.ElideRight : Text.ElideNone
    }
    Label {
        id: subtitleLabel
        anchors {
            left: parent.left
            right: parent.right
        }
        fontSize: "small"
        horizontalAlignment: (captionStyle == Ubuntu.SummaryCaptionStyle) ? Text.AlignRight : Text.AlignLeft
        visible: text !== ""
        maximumLineCount: (captionStyle == Ubuntu.TitleCaptionStyle) ? 2 : 1
        wrapMode: (captionStyle == Ubuntu.TitleCaptionStyle) ? Text.Wrap : Text.NoWrap
        elide: (captionStyle == Ubuntu.TitleCaptionStyle) ? Text.ElideRight : Text.ElideNone
    }
}
