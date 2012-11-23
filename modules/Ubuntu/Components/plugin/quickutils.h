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
 */

#ifndef QUICKUTILS_P_H
#define QUICKUTILS_P_H

#include <QtCore/QObject>
#include <QtQuick/QQuickView>

class QQuickItem;
class QQmlEngine;
class QQmlComponent;
class QJSEngine;
class QuickUtils : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem *rootObject READ rootObject NOTIFY rootObjectChanged)
public:
    static QuickUtils& instance()
    {
        static QuickUtils instance;
        return instance;
    }

    QQuickItem *rootObject();

    Q_INVOKABLE qreal modelDelegateHeight(QQmlComponent *delegate, const QVariant &model);

Q_SIGNALS:
    void rootObjectChanged();

private:
    explicit QuickUtils(QObject *parent = 0);
    QQuickView *m_rootView;

    void lookupQuickView();
};


#endif // QUICKUTILS_P_H
