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

#include "splitviewhandler_p_p.h"
#include <QtQuick/private/qquickanchors_p.h>
#include <QtQuick/private/qquickitem_p.h>
#include <QtQuick/private/qquickevents_p_p.h>
#include <splitview_p_p.h>

UT_NAMESPACE_BEGIN

SplitViewHandler::SplitViewHandler(QQuickItem *parent)
    : QQuickMouseArea(parent)
{
    setFlag(ItemHasContents);
    setHoverEnabled(true);
    setAcceptedButtons(Qt::LeftButton);

    setCursorShape(Qt::SplitHCursor);
    qDebug() << "Anchor to" << parent;
    // attach the handler to the parent's rigth edge
    QQuickItemPrivate *dParent = QQuickItemPrivate::get(parent);
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
    disconnect(*spacing);
    delete spacing;
}

void SplitViewHandler::bindSpacing(SplitView *view)
{
    auto resizer = [view, this]() {
        setWidth(view->spacing());
    };
    spacing = new QMetaObject::Connection;
    *spacing = connect(view, &SplitView::spacingChanged, resizer);
    setWidth(view->spacing());
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

UT_NAMESPACE_END
