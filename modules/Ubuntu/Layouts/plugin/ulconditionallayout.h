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

#ifndef ULCONDITIONALLAYOUT_H
#define ULCONDITIONALLAYOUT_H

#include <QtQml/QQmlComponent>
#include <QtCore/QStringList>
#include <QtQml/QQmlScriptString>

class QQuickItem;
class ULConditionalLayoutAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList items READ items WRITE setItems NOTIFY itemsChanged)

    Q_PROPERTY(QQmlScriptString width READ width WRITE setWidth)
    Q_PROPERTY(QQmlScriptString height READ height WRITE setHeight)
    Q_PROPERTY(QQmlScriptString scale READ scale WRITE setScale)
    Q_PROPERTY(QQmlScriptString rotation READ rotation WRITE setRotation)

public:
    explicit ULConditionalLayoutAttached(QObject *parent = 0);

Q_SIGNALS:
    void itemsChanged();

public: // getter/setter
    QStringList items() const;
    void setItems(const QStringList &names);

    QQmlScriptString width() const;
    void setWidth(const QQmlScriptString &width);
    QQmlScriptString height() const;
    void setHeight(const QQmlScriptString &height);
    QQmlScriptString scale() const;
    void setScale(const QQmlScriptString &scale);
    QQmlScriptString rotation() const;
    void setRotation(const QQmlScriptString &rotation);

private:
    QStringList m_itemNames;
    QQmlScriptString m_width;
    QQmlScriptString m_height;
    QQmlScriptString m_scale;
    QQmlScriptString m_rotation;
};


class QQmlBinding;
class ULConditionalLayoutPrivate;
class ULConditionalLayout : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ULConditionalLayout)

    Q_PROPERTY(QString name READ layoutName WRITE setLayoutName)
    Q_PROPERTY(QQmlBinding* when READ when WRITE setWhen)
    Q_PROPERTY(QQmlComponent *layout READ layout WRITE setLayout)
    Q_CLASSINFO("DefaultProperty", "layout")
public:
    explicit ULConditionalLayout(QObject *parent = 0);
    ~ULConditionalLayout();

    static ULConditionalLayoutAttached * qmlAttachedProperties(QObject *owner);

// getter/setter
    QString layoutName() const;
    void setLayoutName(const QString &name);
    QQmlBinding *when() const;
    void setWhen(QQmlBinding *when);
    QQmlComponent *layout() const;
    void setLayout(QQmlComponent *component);

private:
    Q_DECLARE_PRIVATE(ULConditionalLayout)
    QScopedPointer<ULConditionalLayoutPrivate> d_ptr;
};

QML_DECLARE_TYPEINFO(ULConditionalLayout, QML_HAS_ATTACHED_PROPERTIES)

#endif // ULCONDITIONALLAYOUT_H
