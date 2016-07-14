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

#ifndef SPLITVIEWHANDLER_H
#define SPLITVIEWHANDLER_H

#include <QQuickItem>
#include <QtQuick/private/qquickmousearea_p.h>
#include <ubuntutoolkitglobal.h>

UT_NAMESPACE_BEGIN

class SplitView;
//class QQuickMouseEvent;
class SplitViewHandler : public QQuickMouseArea
{
    Q_OBJECT
public:
    explicit SplitViewHandler(QQuickItem *parent = 0);
    ~SplitViewHandler();
    void bindSpacing(SplitView *view);

protected:
    QMetaObject::Connection *spacing{nullptr};
    QPointF prevPos;

private Q_SLOTS:
    void onPressed(QQuickMouseEvent *event);
    void onReleased(QQuickMouseEvent *event);
    void onPositionChanged(QQuickMouseEvent *event);
};

UT_NAMESPACE_END

#endif // SPLITVIEWHANDLER_H
