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

class PropertyAction
{
public:
    enum Type {Binding, Value};
    PropertyAction(const PropertyAction &other);
    PropertyAction();
    PropertyAction(QObject *item, const QString &name, Type type = Binding);
    PropertyAction(QObject *item, const QString &name, QQmlContext *context, const QVariant &value, Type type = Value);

    void setValue(const QVariant &value);
    void setTargetBinding(QQmlAbstractBinding *binding, bool deletable);
    void apply();
    void reset();
    void revert(bool reset = false);

    // members
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

    PropertyChange(Priority priority);
    PropertyChange(QQuickItem *target, const QString &property, const QVariant &value, Priority priority = Low);
    PropertyChange(QQuickItem *target, const QString &property, const QQmlScriptString &script, QQmlContext *scriptContext, Priority priority = Low);
    virtual ~PropertyChange() {}

    virtual void saveState();
    virtual void apply();
    virtual void revert();

    inline Priority priority() { return actionPriority; }
    inline QQmlProperty property() const
    {
        return action.property;
    }
    inline PropertyAction &actionObject()
    {
        return action;
    }
    inline void resetWhenReverted(bool doReset)
    {
        resetOnRevert = doReset;
    }

protected:
    Priority actionPriority;
    bool resetOnRevert;
    PropertyAction action;
};


class PropertyBackup: public PropertyChange
{
public:
    PropertyBackup(QQuickItem *target, const QString &property);
};


class ParentChange : public PropertyChange
{
public:
    ParentChange(QQuickItem *item, QQuickItem *targetParent, bool topmostChild);

    void apply();
private:
    QQuickItem *newParent;
    bool topmostChild;
};


class AnchorChange : public PropertyChange
{
public:
    AnchorChange(QQuickItem *item, const QString &anchor, QQuickItem *target, const QString &targetAnchor = QString());

    void apply();
    void revert();
private:
    bool active;
};


class ItemStackBackup : public PropertyChange
{
public:
    ItemStackBackup(QQuickItem *item);
    void apply() {}
    void revert();

protected:
    virtual void saveState();
    QQuickItem *target;
    QQuickItem *prevItem;
private:
    friend class ULLayouts;
};


class QQuickAnchors;
class AnchorBackup : public PropertyChange
{
public:
    AnchorBackup(QQuickItem *item);

    void apply();
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

    inline QQuickAnchors::Anchor anchor(Anchor id)
    {
        return (QQuickAnchors::Anchor)(1 << (int)id);
    }

    QQuickAnchors *anchorsObject;
    QQuickAnchors::Anchors used;
    QList<PropertyAction> actions;
};


class ULConditionalLayoutAttached;
class ChangeList
{
public:
    ChangeList(){}
    ~ChangeList();

    void apply();
    void revert();
    void clear();

    ChangeList &addChange(PropertyChange *change);
    ChangeList &addParentChange(QQuickItem *item, QQuickItem *newParent, bool topmostItem);

private:
    QList<PropertyChange*> changes[PropertyChange::MaxPriority];
    QList<PropertyChange*> unifiedChanges();
};

#endif // LAYOUTACTION_P_H
