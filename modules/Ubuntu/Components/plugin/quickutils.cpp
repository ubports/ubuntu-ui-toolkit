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
#include <QtCore/QAbstractProxyModel>
#include <QtQml/QQmlPropertyMap>

#include <private/qquicktextinput_p.h>
#include <private/qquicktextedit_p.h>

#include <QDebug>

QuickUtils::QuickUtils(QObject *parent) :
    QObject(parent)
{
    QGuiApplication::instance()->installEventFilter(this);
}

/*!
 * \internal
 * Filter events to catch ChildAdded, when the the application gets the topmost
 * window assigned. Need to check the topmost windows each time as widgets added
 * to the application are not signaled in any other way.
 */
bool QuickUtils::eventFilter(QObject *obj, QEvent *event)
{
    if (!m_rootView && (event->type() == QEvent::ChildAdded))
        lookupQuickView();

    return QObject::eventFilter(obj, event);
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

QString QuickUtils::inputMethodProvider() const
{
    return QString(getenv("QT_IM_MODULE"));
}


/*!
 * \internal
 * Creates an instance out of a delegate using the roles specified in the
 * modelData. Accepts QAbstractListModel, QAbstractProxyModel, QStringList
 * and QVariantList models.
 */


qreal QuickUtils::modelDelegateHeight(QQmlComponent *delegate, const QVariant &modelData)
{
    qreal result = 0.0;
    if (!delegate)
        return result;

    QQmlContext *creationContext = delegate->creationContext();
    QQmlContext *context = 0;
    QAbstractListModel *model = qvariant_cast<QAbstractListModel*>(modelData);
    QAbstractProxyModel *proxy = qvariant_cast<QAbstractProxyModel*>(modelData);

    if (proxy)
        model = qobject_cast<QAbstractListModel*>(proxy->sourceModel());

    if (model) {
        // QAbstractListModel derived models

        if (model->rowCount() > 0) {
            context = new QQmlContext(creationContext);
            const QHash<int,QByteArray> roles = model->roleNames();
            if (roles.count()) {

                // put roles inside the context
                QHashIterator<int,QByteArray> i(roles);
                while (i.hasNext()) {
                    i.next();
                    context->setContextProperty(i.value(), "");
                }
            }
        }
    } else if (modelData.type() == QVariant::List){
        QVariantList vModel = qvariant_cast<QVariantList>(modelData);
        if (vModel.count() > 0) {
            context = new QQmlContext(creationContext);
            if (vModel[0].type() == QVariant::String) {
                // the only role name we have is modelData
                context->setContextProperty("modelData", "");

            } else if (vModel[0].type() == QVariant::Map) {
                // we need only the keys, attached to an object named "modelData"
                context = new QQmlContext(creationContext);
                QQmlPropertyMap *modelData = new QQmlPropertyMap(context);
                QVariantMap map = qvariant_cast<QVariantMap>(vModel[0]);
                QStringList roles = map.uniqueKeys();

                Q_FOREACH(const QString &role, roles)
                    modelData->insert(role, "");

                context->setContextProperty("modelData", modelData);
            }
        }
    } else if (modelData.type() == QVariant::Int) {
        context = new QQmlContext(creationContext);
        // the only role name we have is modelData
        context->setContextProperty("modelData", 0);
    }
    if (context) {
        // add index and section too
        context->setContextProperty("index", 0);
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
 * Returns the class name (type) of a QtQuick item.
 */
QString QuickUtils::className(QQuickItem *item)
{
    QString result = item->metaObject()->className();
    return result.left(result.indexOf("_QML"));
}


/*!
 * \internal
 * Get QQuickView from the application's window list and connect its status change
 * signal as the root element is set after the root element completion.
 */
void QuickUtils::lookupQuickView()
{
    if (m_rootView)
        return;
    Q_FOREACH (QWindow *w, QGuiApplication::topLevelWindows()) {
        m_rootView = qobject_cast<QQuickView*>(w);
        if (m_rootView) {
            // connect in case we get the root object changed
            QObject::connect(m_rootView, SIGNAL(statusChanged(QQuickView::Status)),
                             this, SIGNAL(rootObjectChanged()));
            // emit changed signal so we update the eventual bindings
            if (m_rootView->rootObject())
                Q_EMIT rootObjectChanged();
            break;
        }
    }
}
