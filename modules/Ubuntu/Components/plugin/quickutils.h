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
    Q_PROPERTY(QString inputMethodProvider READ inputMethodProvider)
public:
    static QuickUtils& instance()
    {
        static QuickUtils instance;
        return instance;
    }

    QQuickItem *rootObject();
    Q_INVOKABLE QQuickItem *rootItem(QObject *object);
    QString inputMethodProvider() const;

    Q_INVOKABLE qreal modelDelegateHeight(QQmlComponent *delegate, const QVariant &model);
    Q_INVOKABLE QString className(QQuickItem *item);
    QObject* createQmlObject(const QUrl &url);
    void setImportPathList(const QStringList &paths);

Q_SIGNALS:
    void rootObjectChanged();
    void deactivated();

protected:
    bool eventFilter(QObject *, QEvent *);

private Q_SLOTS:
    void activeFocus(QObject*);

private:
    explicit QuickUtils(QObject *parent = 0);
    QQuickView *m_rootView;
    QQmlEngine *m_engine;

    void lookupQuickView();
};


#endif // QUICKUTILS_P_H
