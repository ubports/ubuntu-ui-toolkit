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

    // these methods are supposed to be used internally by the styling
    inline static bool omitProperty(const char *name)
    {
        static QString properties(
                    ",activeFocus,anchors,antialiasing,baseline,baselineOffset,bottom,children,"
                    "childrenRect,clip,data,focus,horizontalCenter,layer,left,objectName,parent,"
                    "resources,right,states,top,transform,transformOrigin,transitions,"
                    "verticalCenter,visibleChildren,x,y,");
        return properties.contains(QString(name).prepend(',').append(','));
    }
    int bindItem(QQuickItem *item, StyledPropertyMap &propertyMap, bool usePropertyMap);
    bool unbindItem(QQuickItem *item);
    bool unbindProperty(const QString &property);
    bool isUpdating(const QString &property) const;

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
