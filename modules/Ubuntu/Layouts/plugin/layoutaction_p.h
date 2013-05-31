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

class QmlProperty
{
public:
    QmlProperty() : binding(0) {}
    QmlProperty(QQuickItem *item, const QString &name);
    QmlProperty(const QmlProperty &other);

    void reset();
    bool revert();

    QQmlProperty property;
    QQmlAbstractBinding *binding;
};

class QQuickItem;
class PropertyAction
{
public:
    enum Priority {
        High,
        Normal,
        Low,
        MaxPriority
    };

    PropertyAction(QQuickItem *target, const QString &property, const QVariant &value, Priority priority = Low);
    virtual ~PropertyAction() {}
    virtual void saveState();
    virtual void execute();
    virtual void revert();
    inline Priority priority() { return actionPriority; }

protected:

    static void saveProperty(const QQmlProperty &property, QQmlAbstractBinding **binding, QVariant &value);
    static void restoreProperty(const QQmlProperty &property, QQmlAbstractBinding *binding, const QVariant &value);
    static bool restoreBinding(const QQmlProperty &property, QQmlAbstractBinding *binding);

    Priority actionPriority;
    QQmlAbstractBinding *originalBinding, *targetBinding;
    QQmlProperty targetProperty;
    QVariant originalValue, targetValue;
};


class ParentAction : public PropertyAction
{
public:
    ParentAction(QQuickItem *target, QQuickItem *source);

    virtual void execute();

protected:
    virtual void saveState();

    QQmlProperty sourceProperty;
};


class ReparentAction : public PropertyAction
{
public:
    ReparentAction(QQuickItem *target, QQuickItem *targetParent);

    void execute();
    void revert();
protected:
    virtual void saveState();

    QQmlProperty xProperty;
    QQmlProperty yProperty;
    QQmlProperty widthProperty;
    QQmlProperty heightProperty;
    QQmlProperty scaleProperty;
    QQmlProperty rotationProperty;

    QQuickItem *originalStackBefore;

    QQmlAbstractBinding *xBinding;
    QQmlAbstractBinding *yBinding;
    QQmlAbstractBinding *widthBinding;
    QQmlAbstractBinding *heightBinding;
    QQmlAbstractBinding *scaleBinding;
    QQmlAbstractBinding *rotationBinding;

    QVariant originalX;
    QVariant originalY;
    QVariant originalWidth;
    QVariant originalHeight;
    QVariant originalScale;
    QVariant originalRotation;
};


class AnchorPropertyAction : public PropertyAction
{
public:
    AnchorPropertyAction(QQuickItem *target, const QString &property, const QVariant &value);

    void execute();
    void revert();
protected:
    virtual void saveState();

    QQuickAnchorLine line;
    qreal margin;
};

class QQuickAnchors;
class AnchorBackupAction : public PropertyAction
{
public:
    AnchorBackupAction(QQuickItem *target);

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

    QmlProperty fill;
    QmlProperty centerIn;
    QmlProperty anchors[MaxAnchor];
    QmlProperty margins[MaxMargins];
};


class ActionList
{
public:
    ActionList(){}
    ~ActionList();

    void apply();
    void revert();
    void clear();

    ActionList &operator<<(PropertyAction *action);

private:
    QList<PropertyAction*> changes[PropertyAction::MaxPriority];
};

#endif // LAYOUTACTION_P_H
