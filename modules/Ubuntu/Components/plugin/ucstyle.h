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
#include <QtCore/QLatin1String>
#include <QtCore/QSet>
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
class ItemStyleAttached;
class UCStyle : public QObject
{
    Q_OBJECT
public:
    explicit UCStyle(QObject *parent = 0);
    ~UCStyle();

    // these methods are supposed to be used internally by the styling
    inline static bool omitProperty(const char *name)
    {
        static QSet<QLatin1String> properties = QSet<QLatin1String>() <<
             QLatin1String("activeFocus") << QLatin1String("anchors") << QLatin1String("antialiasing") << QLatin1String("baseline") << QLatin1String("baselineOffset") <<
             QLatin1String("bottom") << QLatin1String("children") << QLatin1String("childrenRect") << QLatin1String("clip") << QLatin1String("data") << QLatin1String("focus") <<
             QLatin1String("horizontalCenter") << QLatin1String("implicitHeight") << QLatin1String("implicitWidth") << QLatin1String("layer") << QLatin1String("left") <<
             QLatin1String("objectName") << QLatin1String("parent") << QLatin1String("resources") << QLatin1String("right") << QLatin1String("rotation") << QLatin1String("scale") <<
             QLatin1String("smooth") << QLatin1String("state") << QLatin1String("states") << QLatin1String("top") << QLatin1String("transform") << QLatin1String("transformOrigin") <<
             QLatin1String("transitions") << QLatin1String( "verticalCenter") << QLatin1String("visibleChildren") << QLatin1String("x") << QLatin1String("y") << QLatin1String("z");
        return properties.contains(QLatin1String(name));
    }
    int bindItem(QQuickItem *item, StyledPropertyMap &propertyMap, bool usePropertyMap);
    bool unbindItem(QQuickItem *item);
    bool unbindProperty(const QString &property);
    bool isUpdating(const QString &property) const;
    bool setStylerObject(ItemStyleAttached *styler);

private Q_SLOTS:
    void updateStyledItem();
    
private:
    QHash<QString, QQmlProperty> m_bindings;
    QString m_propertyUpdated;
    ItemStyleAttached *m_styler;

    void bind(const QQmlProperty &property);
    void unbind(const QString &name);
    void write(const QString &source, const QQmlProperty &destination);
};

#endif // UCSTYLE_H
