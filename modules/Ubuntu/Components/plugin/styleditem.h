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

#ifndef STYLEDITEM_H
#define STYLEDITEM_H

#include <QObject>
#include <QScopedPointer>
#include <QtQml>
#include <QtQuick/QQuickItem>

class StyleRule;

class StyledItemPrivate;
class StyledItem : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QString styleClass READ styleClass WRITE setStyleClass NOTIFY styleChanged)
    Q_PROPERTY(QString instanceId READ instanceId WRITE setInstanceId NOTIFY styleChanged)
    Q_PROPERTY(StyleRule *style READ style WRITE setStyle RESET resetStyle NOTIFY styleChanged)
    Q_PROPERTY(QObject *styleObject READ styleObject NOTIFY styleChanged)
    Q_PROPERTY(QQuickItem *delegateItem READ delegateItem NOTIFY styleChanged)

public:
    StyledItem(QQuickItem *parent = 0);
    ~StyledItem();

protected:
    void componentComplete();
    
Q_SIGNALS:

    void styleChanged();
    
public Q_SLOTS:

public: // getter/setter
    QString instanceId() const;
    void setInstanceId(const QString &instanceId);
    QString styleClass() const;
    void setStyleClass(const QString &styleClass);
    StyleRule *style() const;
    void setStyle(StyleRule *style);
    void resetStyle();
    QObject *styleObject() const;
    QQuickItem *delegateItem() const;

private: //members
    Q_DISABLE_COPY(StyledItem)
    Q_DECLARE_PRIVATE(StyledItem)
    QScopedPointer<StyledItemPrivate> d_ptr;

    Q_PRIVATE_SLOT(d_func(), void _q_reloadTheme())
};

QML_DECLARE_TYPE(StyledItem);

#endif // STYLEDITEM_H
