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
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#ifndef UCSTYLE_H
#define UCSTYLE_H

#include <QtCore/QObject>
#include <QtQml/QQmlProperty>

#define OMIT_STYLED_PROPERTIES    \
    "objectName,parent,children,x,y,z,states,transitions,childrenRect,"\
    "visibleChildren,anchors,left,right,top,bottom,horizontalCenter,"\
    "verticalCenter,baseline,baselineOffset,clip,focus," \
    "activeFocus,rotation,data"

#define OMIT_DELEGATE_PROPERTIES    \
    "objectName,parent,children,x,y,z,states,transitions,childrenRect,"\
    "visibleChildren,verticalCenter,baseline,baselineOffset,clip,focus," \
    "activeFocus,rotation"

typedef QHash<int, bool> StyledPropertyMap;

class QQmlAbstractBinding;
class QQuickItem;
class UCStyle : public QObject
{
    Q_OBJECT
public:
    explicit UCStyle(QObject *parent = 0);
    ~UCStyle();

protected:
    // these methods are supposed to be used internally by the styling
    void bindStyledItem(QQuickItem *item, StyledPropertyMap &propertyMap);
    void bindDelegate(QQuickItem *item, StyledPropertyMap &propertyMap);
    void unbindItem(QQuickItem *item);
    void unbindProperty(const QString &property);
    bool isUpdating(const QString &property);
    friend class ItemStyleAttachedPrivate;

private Q_SLOTS:
    void updateStyledItem();
    
private:
    struct Binding {
        QQmlAbstractBinding *prevBind;
        QQuickItem *target;
        QQmlProperty styledProperty;
    };

    QHash<int, Binding> m_bindings;
    QString m_propertyUpdated;

    void bind(int index, QQuickItem *target, const QQmlProperty &property);
    void unbind(int index);
};

#endif // UCSTYLE_H
