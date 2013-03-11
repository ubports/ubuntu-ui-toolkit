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

#ifndef ITEMSTYLEATTACHED_H
#define ITEMSTYLEATTACHED_H

#include <QtCore/QObject>
#include <QtQml>

class QQuickItem;
class ItemStyleAttachedPrivate;
class ItemStyleAttached : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString class READ styleClass WRITE setStyleClass NOTIFY styleChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY styleChanged)
    Q_PROPERTY(QObject *style READ style WRITE setStyle NOTIFY styleChanged)
    Q_PROPERTY(QQuickItem *delegate READ delegate WRITE setDelegate NOTIFY styleChanged)

    Q_PROPERTY(QString path READ path NOTIFY styleChanged)

public:
    explicit ItemStyleAttached(QObject *parent = 0);
    ~ItemStyleAttached();
    static ItemStyleAttached *qmlAttachedProperties(QObject *obj);

Q_SIGNALS:
    void styleChanged();
    
public Q_SLOTS:

public: //getters
    QString name() const;
    QString styleClass() const;
    QString path() const;

private:
    void setName(const QString &name);
    void setStyleClass(const QString &styleClass);
    QObject *style() const;
    void setStyle(QObject *style);
    QQuickItem *delegate() const;
    void setDelegate(QQuickItem *delegate);

private:
    Q_DISABLE_COPY(ItemStyleAttached)
    Q_DECLARE_PRIVATE(ItemStyleAttached)
    QScopedPointer<ItemStyleAttachedPrivate> d_ptr;
    friend class ThemeEnginePrivate;

    Q_PRIVATE_SLOT(d_func(), void _q_attacheePropertyChanged())
    Q_PRIVATE_SLOT(d_func(), void _q_updateAttacheeProperty())
    Q_PRIVATE_SLOT(d_func(), void _q_updateDelegateProperty())
    Q_PRIVATE_SLOT(d_func(), void _q_reapplyStyling(QQuickItem *))
    Q_PRIVATE_SLOT(d_func(), void _q_refreshStyle())
};

QML_DECLARE_TYPEINFO(ItemStyleAttached, QML_HAS_ATTACHED_PROPERTIES)

#endif // ITEMSTYLEATTACHED_H
