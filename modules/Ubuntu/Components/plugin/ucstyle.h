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

class QQmlAbstractBinding;
typedef QPair<unsigned, QQmlAbstractBinding*> PropertyPair;
class StyledPropertyMap : public QHash<int, PropertyPair> {
public: enum Type {
        Invalid,
        Banned,
        Enabled,
        Bound,
        Styled
    };

public:
    StyledPropertyMap(){}

    inline void mark(int key, Type type, QQmlAbstractBinding *binding = 0)
    {
        insert(key, PropertyPair(type, binding));
    }

    inline QQmlAbstractBinding *binding(int key)
    {
        return value(key).second;
    }

    inline bool isEnabled(int key)
    {
        Type t = (Type)value(key).first;
        return (t >= Enabled);
    }

    inline bool isBanned(int key)
    {
        return (Type)value(key).first == Banned;
    }

    inline bool isStyled(int key)
    {
        return (Type)value(key).first == Styled;
    }
};


class QQmlBinding;
class QQuickItem;
class UCStyle : public QObject
{
    Q_OBJECT
public:
    explicit UCStyle(QObject *parent = 0);
    ~UCStyle();

protected:
    // these methods are supposed to be used internally by the styling
    inline static bool omitStyledProperty(const char *name)
    {
        static QString properties("objectName,parent,children,x,y,z,states,transitions,childrenRect,"
                                    "visibleChildren,anchors,left,right,top,bottom,horizontalCenter,"
                                    "verticalCenter,baseline,baselineOffset,clip,focus,"
                                    "activeFocus,rotation,data");
        return properties.contains(name);
    }
    static bool omitDelegateProperty(const char *name)
    {
        static QString properties("objectName,parent,children,x,y,z,states,transitions,childrenRect,"
                                    "visibleChildren,verticalCenter,baseline,baselineOffset,clip,focus,"
                                    "activeFocus");
        return properties.contains(name);
    }
    void bindStyledItem(QQuickItem *item, StyledPropertyMap &propertyMap);
    void bindDelegate(QQuickItem *item, StyledPropertyMap &propertyMap);
    void unbindItem(QQuickItem *item);
    void unbindProperty(const QString &property);
    bool isUpdating(const QString &property) const;
    friend class ItemStyleAttachedPrivate;

private Q_SLOTS:
    void updateStyledItem();
    
private:
    QHash<QString, QQmlProperty> m_bindings;
    QString m_propertyUpdated;

    void bind(const QQmlProperty &property);
    void unbind(const QString &name);
    void write(const QString &source, const QQmlProperty &destination);
};

#endif // UCSTYLE_H
