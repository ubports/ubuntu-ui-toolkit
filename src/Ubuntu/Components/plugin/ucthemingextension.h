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
#include <QtQml>

class QQuickItem;
class UCStyledItemBase;
class UCTheme;
class UCThemingExtension;
class UCItemAttached : public QObject
{
    Q_OBJECT
public:
    explicit UCItemAttached(QObject *owner = 0);
    ~UCItemAttached();
    static bool isThemed(QQuickItem *item);
    static UCItemAttached *qmlAttachedProperties(QObject *owner);

    QQuickItem *m_item;
    QQuickItem *m_prevParent;
    UCThemingExtension *m_extension;

private:
    Q_SLOT void handleParentChanged(QQuickItem *newParent);
    Q_SLOT void reloadTheme();

    friend class UCThemingExtension;
};
QML_DECLARE_TYPEINFO(UCItemAttached, QML_HAS_ATTACHED_PROPERTIES)

class UCThemeEvent : public QEvent
{
public: // statics
    // event ID
    static int themeUpdatedId;
    static int themeReloadedId;
    static bool isThemeEvent(const QEvent *event);
    static void handleEvent(QQuickItem *item, UCThemeEvent *event, bool synchronous);
    static void forwardEvent(QQuickItem *item, UCThemeEvent *event);
    static void broadcastThemeChange(QQuickItem *item, UCTheme *oldTheme, UCTheme *newTheme);
    static void broadcastThemeReloaded(QQuickItem *item, UCTheme *theme);

public:
    explicit UCThemeEvent(UCTheme *reloadedTheme);
    UCThemeEvent(UCTheme *oldTheme, UCTheme *newTheme);
    UCThemeEvent(const UCThemeEvent &other);

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

class UCThemingExtension
{
public:
    enum ThemeType {
        Inherited,
        Custom
    };

    explicit UCThemingExtension();

    virtual void preThemeChanged() = 0;
    virtual void postThemeChanged() = 0;

    virtual void initTheming(QQuickItem *item);
    virtual void handleThemeEvent(UCThemeEvent *event);

    UCTheme *getTheme() const;
    void setTheme(UCTheme *newTheme, ThemeType type = Custom);
    void resetTheme();

    static QQuickItem *ascendantThemed(QQuickItem *item);

    QQuickItem *themedItem;
    UCItemAttached *attachedThemer;
    UCTheme *theme;
    ThemeType themeType;

    void setParentTheme();
};

#endif // UCITEMEXTENSION_H
