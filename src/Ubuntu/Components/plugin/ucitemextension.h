/*
 * Copyright 2015 Canonical Ltd.
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

#ifndef UCITEMEXTENSION_H
#define UCITEMEXTENSION_H

#include <QtCore/QObject>
#include <QtCore/QEvent>
#include <QtCore/QPointer>

class QQuickItem;
class UCStyledItemBase;
class UCTheme;
class UCItemExtension : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem *parent READ parentItem WRITE setParentItem NOTIFY extendedParentChanged)
public:
    explicit UCItemExtension(QObject *parent = 0);

    QQuickItem *parentItem() const;
    void setParentItem(QQuickItem *parentItem);

Q_SIGNALS:
    void extendedParentChanged();

private:
    QQuickItem *m_item;
    QQuickItem *m_prevParent;

    Q_SLOT void handleParentChanged(QQuickItem *newParent);
};

class UCThemeUpdateEvent : public QEvent
{
public: // statics
    // event ID
    static int themeEventId;
    static void handleEvent(QQuickItem *item, UCTheme *oldTheme, UCTheme *newTheme);
    static void broadcastToChildren(QQuickItem *item, UCTheme *oldTheme, UCTheme *newTheme);
    static void forwardEvent(QQuickItem *item, UCThemeUpdateEvent *event);

public:
    explicit UCThemeUpdateEvent(UCTheme *oldTheme, UCTheme *newTheme);

    UCTheme *oldTheme() const
    {
        return m_oldTheme;
    }
    UCTheme *newTheme() const
    {
        return m_newTheme;
    }
private:
    UCTheme *m_oldTheme;
    UCTheme *m_newTheme;
};

#endif // UCITEMEXTENSION_H
