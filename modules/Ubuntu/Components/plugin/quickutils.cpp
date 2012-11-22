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
#include <QtQml/QQmlContext>
#include <QtCore/QAbstractListModel>

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
 * Creates an instance out of a delegate using the roles specified in the
 * modelData. Accepts QAbstractListModel delegates
 */
qreal QuickUtils::modelDelegateHeight(QQmlComponent *delegate, const QVariant &modelData)
{
    QAbstractListModel *model = qvariant_cast<QAbstractListModel*>(modelData);
    // accept only QAbstractListModel derived models
    if (!delegate || !model)
        return 0.0;

    QQmlContext *creationContext = delegate->creationContext();
    QQmlContext *context = new QQmlContext(creationContext);

    qreal result = 0.0;
    // define roles inside the context
    const QHash<int,QByteArray> roles = model->roleNames();
    if (roles.count()) {
        QHashIterator<int,QByteArray> i(roles);
        while (i.hasNext()) {
            i.next();
            context->setContextProperty(i.value(), "");
        }
        // add section too
        context->setContextProperty("section", "");
        // create item from component
        QObject * obj = delegate->create(context);
        QQuickItem *item = qobject_cast<QQuickItem*>(obj);
        result = item->height();
        item->deleteLater();
    }

    return result;
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
