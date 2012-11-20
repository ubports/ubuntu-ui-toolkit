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
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief Section counter for ListView models.
*/

QtObject {

    /*!
      \preliminary
      The property holds the ListView instance whose model sections are to be counted.
    */
    property var view: null

    /*!
      \preliminary
      The property contains the section counts of the given view.
    */
    property int sectionCount: 0

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
    property var sectionCache: [""]

    function __initSectionCounter()
    {
        if (view.model.countChanged)
            view.model.countChanged.connect(__checkSections);

        if (view.model.itemsChanged)
            view.model.itemsChanged.connect(__checkSections);

        if (view.model.itemsInserted)
            view.model.itemsInserted.connect(__checkSections);

        if (view.model.itemsMoved)
            view.model.itemsMoved.connect(__checkSections);

        if (view.model.itemsRemoved)
            view.model.itemsRemoved.connect(__checkSections);
    }
    function __checkSections()
    {
        function sectionString(str)
        {
            return (view.section.criteria === ViewSection.FirstCharacter) ? str.charAt(0) : str;
        }

        var sections = 0, sectionStack = [];
        var current = "",
                prop = view.section.property,
                item, section = "";
        for (var i = 0, count = (typeof view.model.count === 'undefined' ? view.model.length : view.model.count); i < count; i++) {
            item = view.model.get(i);
            section = sectionString(JSON.stringify(item[prop])).toLowerCase();
            if (section !== current) {
                current = section;
                sections++;
                if (cacheSections)
                    sectionStack.push(current);
            }
        }
        sectionCount = sections;
        if (cacheSections)
            sectionCache = sectionStack;
    }

    onViewChanged: {
        if (view && view.model) {
            __initSectionCounter();
        } else if (view) {
            view.modelChanged.connect(function()
            {
                if (view.model)
                    __initSectionCounter();
            });
        }
    }
}
