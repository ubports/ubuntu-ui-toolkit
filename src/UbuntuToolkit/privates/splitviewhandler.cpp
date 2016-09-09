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
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#include "privates/splitviewhandler_p.h"

#include <QtQml/QQmlEngine>
#include <QtQml/QQmlInfo>
#include <QtQuick/private/qquickanchors_p.h>
#include <QtQuick/private/qquickitem_p.h>
#include <QtQuick/private/qquickevents_p_p.h>

#include "splitview_p_p.h"

UT_NAMESPACE_BEGIN

SplitViewHandler::SplitViewHandler(QQuickItem *parent)
    : QQuickMouseArea(parent)
{
    // for testing purposes
    setObjectName("resize_handle");
    setFlag(ItemHasContents);
    setHoverEnabled(true);
    setAcceptedButtons(Qt::LeftButton);

    setCursorShape(Qt::SplitHCursor);
    setParentItem(parent);

    // attach the handler to the parent's rigth edge
    QQuickItemPrivate *dParent = QQuickItemPrivate::get(parentItem());
    QQuickAnchors *anchors = QQuickItemPrivate::get(this)->anchors();
    anchors->setTop(dParent->top());
    anchors->setBottom(dParent->bottom());
    anchors->setLeft(dParent->right());

    // capture mouse events
    connect(this, SIGNAL(pressed(QQuickMouseEvent*)), this, SLOT(onPressed(QQuickMouseEvent*)));
    connect(this, &SplitViewHandler::released, this, &SplitViewHandler::onReleased);
    connect(this, &SplitViewHandler::positionChanged, this, &SplitViewHandler::onPositionChanged);
}

SplitViewHandler::~SplitViewHandler()
{
    if (spacing) {
        disconnect(*spacing);
    }
    delete spacing;
}

void SplitViewHandler::connectToView(SplitView *view)
{
    this->view = view;
    // grab the context of the parent
    QQmlEngine::setContextForObject(this, qmlContext(view));

    // bind SplitView spacing, use it to specify the resize handle
    auto resizer = [view, this]() {
        setWidth(view->spacing());
    };
    spacing = new QMetaObject::Connection;
    *spacing = connect(view, &SplitView::spacingChanged, resizer);
    setWidth(view->spacing());

    // connect to receive handle delegate
    connect(view, &SplitView::handleDelegateChanged,
            this, &SplitViewHandler::onDelegateChanged);

    onDelegateChanged();
}

void SplitViewHandler::onPressed(QQuickMouseEvent *event)
{
    prevPos = QPointF(event->x(), event->y());
}

void SplitViewHandler::onReleased(QQuickMouseEvent *event)
{
    Q_UNUSED(event);
    prevPos = QPointF();
}

void SplitViewHandler::onPositionChanged(QQuickMouseEvent *event)
{
    if (!pressed()) {
        return;
    }
    qreal dx = event->x() - prevPos.x();
    SplitViewAttached *attached = SplitViewAttached::get(parentItem());
    if (attached) {
        attached->resize(dx);
    }
}

void SplitViewHandler::onDelegateChanged()
{
    static bool warningShown = false;
    // the child is an instance of the delegate
    QList<QQuickItem*> children = childItems();
    qDeleteAll(children);

    // and set the new delegate - if any
    if (SplitViewPrivate::get(view)->handleDelegate) {
        QQmlContext *context = new QQmlContext(qmlContext(this), this);
        context->setContextProperty("handle", QVariant::fromValue(this));
        QObject *object = SplitViewPrivate::get(view)->handleDelegate->beginCreate(context);
        if (object) {
            QQuickItem *item = qobject_cast<QQuickItem*>(object);
            if (!item) {
                if (!warningShown) {
                    qmlInfo(view) << "handle delegate not an Item";
                    warningShown = true;
                }
                SplitViewPrivate::get(view)->handleDelegate->completeCreate();
                delete object;
            } else {
                warningShown = false;
                QQml_setParent_noEvent(item, this);
                item->setParentItem(this);
                SplitViewPrivate::get(view)->handleDelegate->completeCreate();
            }
        }
    }
}

UT_NAMESPACE_END
