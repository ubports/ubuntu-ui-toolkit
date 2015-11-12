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
 *
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#ifndef UCBOTTOMEDGE_H
#define UCBOTTOMEDGE_H

#include "ucstyleditembase.h"
#include <QtCore/QPointer>
#include <QtQuick/private/qquickitemchangelistener_p.h>

class UCBottomEdgeHint;
class UCBottomEdgeRange;
class QQuickAbstractAnimation;
class UCBottomEdgePrivate;
class UCBottomEdge : public UCStyledItemBase
{
    Q_OBJECT
    Q_ENUMS(State DragDirection)

    Q_PROPERTY(UCBottomEdgeHint* hint READ hint CONSTANT FINAL)
    Q_PROPERTY(qreal dragProgress READ dragProgress NOTIFY dragProggressChanged FINAL)
    Q_PROPERTY(DragDirection dragDirection READ dragDirection NOTIFY dragDirectionChanged FINAL)
    Q_PROPERTY(State state READ state NOTIFY stateChanged FINAL)
    Q_PROPERTY(QUrl content READ content WRITE setContent NOTIFY contentChanged FINAL)
    Q_PROPERTY(QQmlComponent *contentComponent READ contentComponent WRITE setContentComponent NOTIFY contentComponentChanged FINAL)
    Q_PROPERTY(QQuickItem* contentItem READ contentItem NOTIFY contentItemChanged FINAL)
    Q_PROPERTY(QQmlListProperty<UCBottomEdgeRange> ranges READ ranges NOTIFY rangesChanged FINAL)
    Q_PROPERTY(UCBottomEdgeRange* activeRange READ activeRange NOTIFY activeRangeChanged FINAL)
    Q_PROPERTY(qreal commitPoint READ commitPoint WRITE setCommitPoint NOTIFY commitPointChanged FINAL)

    // overloaded data property to catch ranges
    Q_PRIVATE_PROPERTY(UCBottomEdge::d_func(), QQmlListProperty<QObject> data READ data DESIGNABLE false)
    Q_CLASSINFO("DefaultProperty", "data")
public:
    enum State {
        Hidden,
        Revealed,
        Committed
    };
    enum DragDirection {
        Undefined,
        Upwards,
        Downwards
    };

    explicit UCBottomEdge(QQuickItem *parent = 0);
    virtual ~UCBottomEdge();

    UCBottomEdgeHint *hint() const;
    qreal dragProgress();
    DragDirection dragDirection() const;
    State state() const;
    QUrl content() const;
    void setContent(const QUrl &url);
    QQmlComponent *contentComponent() const;
    void setContentComponent(QQmlComponent *component);
    QQuickItem *contentItem() const;
    void setFillWindow(bool fill);
    QQmlListProperty<UCBottomEdgeRange> ranges();
    UCBottomEdgeRange *activeRange();
    qreal commitPoint() const;
    void setCommitPoint(qreal point);

Q_SIGNALS:
    void hintChanged();
    void dragProggressChanged();
    void dragDirectionChanged();
    void stateChanged(State state);
    void contentChanged(const QUrl url);
    void contentComponentChanged(QQmlComponent *component);
    void contentItemChanged();
    void fillWindowChanged();
    void rangesChanged();
    void activeRangeChanged();
    void commitPointChanged();

    void commitStarted();
    void commitCompleted();
    void collapseStarted();
    void collapseCompleted();

public Q_SLOTS:
    void commit();
    void collapse();

protected:
    static void ranges_append(QQmlListProperty<UCBottomEdgeRange> *sections, UCBottomEdgeRange *section);
    static int ranges_count(QQmlListProperty<UCBottomEdgeRange> *sections);
    static UCBottomEdgeRange *ranges_at(QQmlListProperty<UCBottomEdgeRange> *sections, int index);
    static void ranges_clear(QQmlListProperty<UCBottomEdgeRange> *sections);

    void classBegin();
    void componentComplete();
    void itemChange(ItemChange change, const ItemChangeData &data);

    void emitCommitCompleted(bool running);
    void emitCollapseCompleted(bool running);

    Q_DECLARE_PRIVATE(UCBottomEdge)
};

#endif // UCBOTTOMEDGE_H
