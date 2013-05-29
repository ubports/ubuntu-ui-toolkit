/*
 * Copyright 2013 Canonical Ltd.
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

#ifndef LAYOUTACTION_P_H
#define LAYOUTACTION_P_H

#include <QtCore/QVariant>
#include <QtQml/QQmlListProperty>

#define foreach Q_FOREACH //workaround to fix private includes
#include <QtQml/private/qqmlbinding_p.h>     // for QmlBinding
#undef foreach
#include <QtQuick/private/qquickanchors_p.h>
#include <QtQuick/private/qquickanchors_p_p.h>

class QQuickItem;

class LayoutAction
{
public:
    enum ActionType {
        Invalid,
        Breaker,
        Updater
    };

    LayoutAction(ActionType type = Invalid);
    virtual ~LayoutAction() {}
    virtual void execute();
    virtual void revert();
    inline ActionType type() { return actionType; }
protected:
    void initialize();

    ActionType actionType;
    QQmlAbstractBinding *originalBinding, *targetBinding;
    QQmlProperty targetProperty;
    QVariant originalValue, targetValue;
};


class ActionList
{
public:
    ActionList(){}
    ~ActionList();

    void apply();
    void revert();
    void clear();

    ActionList &operator<<(LayoutAction *action);

private:
    QList<LayoutAction*> breakerChanges;
    QList<LayoutAction*> updaterChanges;
};


class PropertyAction : public LayoutAction
{
public:
    PropertyAction(QQuickItem *target, const QString &property, const QVariant &value);
};


class ParentAction : public LayoutAction
{
public:
    ParentAction(QQuickItem *target, const QString &property, QQuickItem *source);

    virtual void execute();

protected:
    QQmlProperty sourceProperty;
};


class AnchorAction : public LayoutAction
{
public:
    AnchorAction(QQuickItem *target, const QString &property);

    void execute();
};

class QQuickAnchors;
class AnchorResetAction : public LayoutAction
{
public:
    AnchorResetAction(QQuickItem *target);

    void execute();
    void revert();
protected:
    QQuickAnchors *anchors;
    QHash<QQuickAnchors::Anchor, QVariant> anchorLines;
//    qreal margins[ElementsSize];
//    QQuickItem *originalFill;
//    QQuickItem *originalCenterIn;
//    bool alignWhenCentered;
};

#endif // LAYOUTACTION_P_H
