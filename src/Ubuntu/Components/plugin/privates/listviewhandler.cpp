/*
 * Copyright 2016 Canonical Ltd.
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
 * Author Zsombor Egri <zsombor.egri@canonical.com>
 */

#include "listviewhandler.h"
#include <QtQuick/QQuickItem>
#include <QtQuick/private/qquickflickable_p.h>

ListViewHandler::ListViewHandler(QObject *parent)
    : QObject(parent)
{
}

int ListViewHandler::count(QQuickFlickable *listView)
{
    return listView->property("count").toInt();
}

QQuickItem *ListViewHandler::currentItem(QQuickFlickable *listView)
{
    return listView->property("currentItem").value<QQuickItem*>();
}

int ListViewHandler::currentIndex(QQuickFlickable *listView)
{
    return listView->property("currentIndex").toInt();
}

void ListViewHandler::setCurrentIndex(QQuickFlickable *listView, int index)
{
    listView->setProperty("currentIndex", index);
}

