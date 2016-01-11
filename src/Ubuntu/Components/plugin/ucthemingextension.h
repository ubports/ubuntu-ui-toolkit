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
class UCTheme;
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
    virtual void itemThemeChanged(UCTheme *, UCTheme*);
    virtual void itemThemeReloaded(UCTheme *);

    UCTheme *getTheme();
    void setTheme(UCTheme *newTheme, ThemeType type = Custom);
    void resetTheme();

    static bool isThemed(QQuickItem *item);
    static QQuickItem *ascendantThemed(QQuickItem *item);

private:
    QPointer<UCTheme> theme;
    QQuickItem *themedItem;
    ThemeType themeType;

    void setParentTheme();
};

#define UCThemingExtension_iid "org.qt-project.Qt.UCThemingExtension"
Q_DECLARE_INTERFACE(UCThemingExtension, UCThemingExtension_iid)

#endif // UCITEMEXTENSION_H
