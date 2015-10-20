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

#include <QtQuick/private/qquickitem_p.h>

class QQuickItem;
class UCStyledItemBase;
class UCTheme;
class UCThemingExtension;
class UCItemAttached : public QObjectUserData, public QQuickItemChangeListener
{
public:
    explicit UCItemAttached(QQuickItem *owner = 0);
    ~UCItemAttached();

    QQuickItem *m_item;
    QQuickItem *m_prevParent;

    void itemParentChanged(QQuickItem *item, QQuickItem *newParent);

private:

    friend class UCThemingExtension;
};

class UCThemeEvent : public QEvent
{
public: // statics
    static bool isThemeEvent(const QEvent *event);

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

    explicit UCThemingExtension(QQuickItem *extendedItem);
    virtual ~UCThemingExtension();

    virtual void preThemeChanged() = 0;
    virtual void postThemeChanged() = 0;

    virtual void handleThemeEvent(UCThemeEvent *event);

    UCTheme *getTheme() const;
    void setTheme(UCTheme *newTheme, ThemeType type = Custom);
    void resetTheme();

    static bool isThemed(QQuickItem *item);
    static QQuickItem *ascendantThemed(QQuickItem *item);

    static void forwardEvent(QQuickItem *item, UCThemeEvent *event);
    static void broadcastThemeChange(QQuickItem *item, UCTheme *oldTheme, UCTheme *newTheme);
    static void broadcastThemeReloaded(QQuickItem *item, UCTheme *theme);

protected:
    QPointer<UCTheme> theme;
    QQuickItem *themedItem;
    ThemeType themeType;

    void setParentTheme();
};

#define UCThemingExtension_iid "org.qt-project.Qt.UCThemingExtension"
Q_DECLARE_INTERFACE(UCThemingExtension, UCThemingExtension_iid)


#endif // UCITEMEXTENSION_H
