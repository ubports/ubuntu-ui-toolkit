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
#include <QtQuick/private/qquickanchors_p_p.h>
#include <QtQuick/private/qquickstate_p.h>

class LayoutAction
{
public:
    enum Type {Binding, Value};
    LayoutAction(const LayoutAction &other);
    LayoutAction();
    LayoutAction(QObject *item, const QString &name, Type type = Binding);
    LayoutAction(QObject *item, const QString &name, QQmlContext *context, const QVariant &value, Type type = Value);

    void setValue(const QVariant &value);
    void setTargetBinding(QQmlAbstractBinding *binding);
    void apply();
    void reset();
    void revert(bool reset = false);
public: // members
    Type type;
    QQmlProperty property;
    QQmlAbstractBinding *fromBinding;
    QWeakPointer<QQmlAbstractBinding> toBinding;
    QVariant fromValue;
    QVariant toValue;

    bool toValueSet:1;
    bool deleteFromBinding:1;
    bool deleteToBinding:1;
};

class QQuickItem;
class PropertyChange
{
public:
    enum Priority {
        High,
        Normal,
        Low,
        MaxPriority
    };

    PropertyChange(QQuickItem *target, const QString &property, const QVariant &value, Priority priority = Low);
    virtual ~PropertyChange() {}

    virtual void saveState();
    virtual void execute();
    virtual void revert();
    inline Priority priority() { return actionPriority; }

protected:

//    static void saveProperty(const QQmlProperty &property, QQmlAbstractBinding **binding, QVariant &value);
//    static void restoreProperty(const QQmlProperty &property, QQmlAbstractBinding *binding, const QVariant &value);
//    static bool restoreBinding(const QQmlProperty &property, QQmlAbstractBinding *binding);

    Priority actionPriority;
    LayoutAction action;
};


class ReparentChange : public PropertyChange
{
public:
    ReparentChange(QQuickItem *target, QQuickItem *source);

    virtual void execute();

protected:
    virtual void saveState();

    QQmlProperty sourceProperty;
};


class ParentChange : public PropertyChange
{
public:
    ParentChange(QQuickItem *target, QQuickItem *targetParent);

    void execute();
    void revert();
protected:
    virtual void saveState();

    QQuickItem *originalStackBefore;
    QQuickItem *container;

    QList<LayoutAction> actions;
};


class QQuickAnchors;
class AnchorChange : public PropertyChange
{
public:
    AnchorChange(QQuickItem *target);

    void execute();
    void revert();
protected:
    virtual void saveState();

    enum Anchor{
        Left = 0,
        Right,
        Top,
        Bottom,
        HCenter,
        VCenter,
        Baseline,
        MaxAnchor
    };
    enum Margins{
        Margins = 0,
        LeftMargin,
        RightMargin,
        TopMargin,
        BottomMargin,
        HCenterOffset,
        VCenterOffset,
        BaselineOffset,
        MaxMargins
    };

    inline QQuickAnchors::Anchor anchor(Anchor id) {
        return (QQuickAnchors::Anchor)(1 << (int)id);
    }

    QQuickAnchors *anchorsObject;
    QQuickAnchors::Anchors used;
    QList<LayoutAction> actions;
};


class ChangeList
{
public:
    ChangeList(){}
    ~ChangeList();

    void apply();
    void revert();
    void clear();

    ChangeList &operator<<(PropertyChange *action);

private:
    QList<PropertyChange*> changes[PropertyChange::MaxPriority];
};

#endif // LAYOUTACTION_P_H
