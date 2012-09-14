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
 * Author: Juhapekka Piiroinen <juhapekka.piiroinen@canonical.com>
 */

#ifndef STYLEDITEM_H
#define STYLEDITEM_H

#include <QDeclarativeItem>
#include <QDeclarativeListProperty>

class Style;

class StyledItemPrivate;
class StyledItem : public QDeclarativeItem
{
    Q_OBJECT
    Q_DISABLE_COPY(StyledItem)

    Q_PROPERTY(QString instanceId READ instanceId WRITE setInstanceId NOTIFY styleChanged)
    Q_PROPERTY(QString styleClass READ styleClass WRITE setStyleClass NOTIFY styleChanged)
    Q_PROPERTY(QString selector READ selector WRITE setSelector NOTIFY styleChanged)
    Q_PROPERTY(Style *style READ activeStyle WRITE setActiveStyle NOTIFY styleChanged FINAL)

public:
    StyledItem(QDeclarativeItem *parent = 0);
    ~StyledItem();

    void componentComplete();
    
signals:

    void styleChanged();
    
public slots:

private: // getter/setter
    QString instanceId() const;
    void setInstanceId(const QString &instanceId);
    QString styleClass() const;
    void setStyleClass(const QString &styleClass);
    QString selector() const;
    void setSelector(const QString &selector);
    Style *activeStyle() const;
    void setActiveStyle(Style *style);

private: //members
    Q_DECLARE_PRIVATE(StyledItem)
    QScopedPointer<StyledItemPrivate> d_ptr;
    friend class ThemeEngine;
    Q_PRIVATE_SLOT(d_func(), void _q_updateCurrentStyle(const QString &))
};

QML_DECLARE_TYPE(StyledItem)

#endif // STYLEDITEM_H
