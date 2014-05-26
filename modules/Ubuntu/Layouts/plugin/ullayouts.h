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

#ifndef ULLAYOUTS_H
#define ULLAYOUTS_H

#include <QtQuick/QQuickItem>
#include "ulconditionallayout.h"

class ULLayoutsAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString item READ item WRITE setItem NOTIFY itemChanged)

public:
    explicit ULLayoutsAttached(QObject *parent = 0);

    //getter/setter
    QString item() const;
    void setItem(const QString &name);

    bool isValid();

Q_SIGNALS:
    void itemChanged();

private Q_SLOTS:
    void validateAttachedProperties();

private:
    QString m_name;
    bool m_valid;
};

class ULLayoutsPrivate;
class ULLayouts : public QQuickItem
{
    Q_OBJECT
    Q_DISABLE_COPY(ULLayouts)

    Q_PROPERTY(QString currentLayout READ currentLayout NOTIFY currentLayoutChanged DESIGNABLE false)
    Q_PROPERTY(QQmlListProperty<ULConditionalLayout> layouts READ layouts DESIGNABLE false)

    Q_PROPERTY(QQmlListProperty<QObject> data READ data DESIGNABLE false)
    Q_PROPERTY(QQmlListProperty<QQuickItem> children READ children DESIGNABLE false)
    Q_CLASSINFO("DefaultProperty", "data")
public:
    explicit ULLayouts(QQuickItem *parent = 0);
    ~ULLayouts();

    static ULLayoutsAttached * qmlAttachedProperties(QObject *owner);

    QString currentLayout() const;
    QList<ULConditionalLayout*> layoutList();
    QQuickItem *contentItem() const;

Q_SIGNALS:
    void currentLayoutChanged();

protected:
    void componentComplete();
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);

private:
    QQmlListProperty<ULConditionalLayout> layouts();
    QQmlListProperty<QObject> data();
    QQmlListProperty<QQuickItem> children();

    friend class ULConditionalLayout;
    Q_DECLARE_PRIVATE(ULLayouts)
    QScopedPointer<ULLayoutsPrivate> d_ptr;
};

QML_DECLARE_TYPE(ULLayouts)
QML_DECLARE_TYPEINFO(ULLayouts, QML_HAS_ATTACHED_PROPERTIES)


#endif // ULLAYOUTS_H

