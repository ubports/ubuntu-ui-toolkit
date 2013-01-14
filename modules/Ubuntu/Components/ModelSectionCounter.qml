/*
 * Copyright 2012 Canonical Ltd.
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

import QtQuick 2.0

/*!
    \qmltype ModelSectionCounter
    \internal
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief Section counter for ListView models.
*/

Object {
    id: counter

    /*!
      \preliminary
      The property holds the ListView instance whose model sections are to be counted.
    */
    property var view: null

    /*!
      \preliminary
      The property contains the section counts of the given view.
    */
    property int count: 0

    /*!
      \preliminary
      The property contains the section Item height.
    */
    readonly property alias sectionHeight: internals.sectionHeight

    /*!
      \preliminary
      When set (true), the sections will be cached for further use. This is used on
      section scrollers (e.g. alphabet scrollbar). The default value is false, meaning
      no caching is done, just counting.
    */
    property bool cacheSections: false

    /*!
      \preliminary
      This property holds the cached sections when the cacheSections property is set, and
      is an empty list when no caching is requested.
    */
    property var cache: []

    /*!
      \internal
      */
    onCacheSectionsChanged: internals.checkSections()

    QtObject {
        id: internals

        property real sectionHeight: (count > 0 && view.section.delegate) ?
                                         QuickUtils.modelDelegateHeight(view.section.delegate, view.model) : 0.0

        property var myView: null
        function disconnectPreviousView()
        {
            if (!myView)
                return;
            if (myView.model.countChanged)
                myView.model.countChanged.disconnect(checkSections);

            if (myView.model.itemsChanged)
                myView.model.itemsChanged.disconnect(checkSections);

            if (myView.model.itemsInserted)
                myView.model.itemsInserted.disconnect(checkSections);

            if (myView.model.itemsMoved)
                myView.model.itemsMoved.disconnect(checkSections);

            if (myView.model.itemsRemoved)
                myView.model.itemsRemoved.disconnect(checkSections);

            if (myView.section.delegateChanged)
                myView.section.delegateChanged.disconnect(checkSections);
        }

        function initSectionCounter()
        {
            myView = view;
            if (myView.model.countChanged)
                myView.model.countChanged.connect(checkSections);

            if (myView.model.itemsChanged)
                myView.model.itemsChanged.connect(checkSections);

            if (myView.model.itemsInserted)
                myView.model.itemsInserted.connect(checkSections);

            if (myView.model.itemsMoved)
                myView.model.itemsMoved.connect(checkSections);

            if (myView.model.itemsRemoved)
                myView.model.itemsRemoved.connect(checkSections);

            if (myView.section.delegateChanged)
                myView.section.delegateChanged.connect(checkSections);

            // finally check sections
            checkSections();
        }

        function checkSections()
        {
            var sections = 0, sectionStack = [];
            if (view && view.section && undefined !== view.section.property && "" !== view.section.property) {
                function sectionString(str)
                {
                    if (str === undefined)
                        return "";
                    return (view.section.criteria === ViewSection.FirstCharacter) ? str.charAt(1) : str;
                }

                var current = "",
                        prop = view.section.property,
                        item = null, section = "",
                        count = (typeof view.model.length === 'undefined' ? view.model.count : view.model.length);
                for (var i = 0; i < count; i++) {
                    if (view.model.hasOwnProperty("get")) {
                        item = view.model.get(i);
                        section = sectionString(JSON.stringify(item[prop])).toLowerCase();
                    } else {
                        item = view.model[i];
                        if (item.hasOwnProperty(prop))
                            section = sectionString(JSON.stringify(item[prop])).toLowerCase();
                        else
                            section = sectionString(JSON.stringify(item)).toLowerCase();
                    }
                    if (section !== current) {
                        current = section;
                        sections++;
                        if (cacheSections)
                            sectionStack.push(current);
                    }
                }
            }
            counter.count = sections;
            counter.cache = sectionStack;
        }
    }

    Component.onCompleted: cacheSectionsChanged.connect(internals.checkSections);

    onViewChanged: {
        internals.disconnectPreviousView()
        if (view && view.hasOwnProperty("model")) {
            if (view && view.model) {
                internals.initSectionCounter();
            } else if (view) {
                view.modelChanged.connect(function()
                {
                    if (view && view.model)
                        internals.initSectionCounter();
                });
            }
        }
    }
}
