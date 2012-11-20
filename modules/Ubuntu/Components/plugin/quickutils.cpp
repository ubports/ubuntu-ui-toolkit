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
 *
 */

#include "quickutils.h"

#include <QGuiApplication>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>

QuickUtils::QuickUtils(QObject *parent) :
    QObject(parent)
{
    lookupQuickView();
}

/*!
 * \internal
 * Returns the current root object.
 */
QQuickItem *QuickUtils::rootObject()
{
    if (!m_rootView)
        lookupQuickView();
    return (m_rootView) ? m_rootView->rootObject() : 0;
}

/*!
 * \internal
 * Get QQuickView from the application's window list and connect its status change
 * signal as the root element is set after the root element completion.
 */
void QuickUtils::lookupQuickView()
{
    Q_FOREACH (QWindow *w, QGuiApplication::allWindows()) {
        m_rootView = qobject_cast<QQuickView*>(w);
        if (m_rootView) {
            QObject::connect(m_rootView, SIGNAL(statusChanged(QQuickView::Status)),
                             this, SIGNAL(rootObjectChanged()));
            break;
        }
    }
}
