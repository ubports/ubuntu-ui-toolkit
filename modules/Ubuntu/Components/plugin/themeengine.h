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

#ifndef THEMEENGINE_H
#define THEMEENGINE_H

#include <QtCore/QObject>
#include <QtCore/QUrl>
#include "stylecache_p.h"

class QQmlEngine;
class QJSEngine;
class QQuickItem;
class ItemStyleAttached;

class ThemeEnginePrivate;
class ThemeEngine : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString error READ error RESET resetError NOTIFY errorChanged)
    Q_PROPERTY(QString currentTheme READ theme NOTIFY themeChanged)
protected:
    ThemeEngine(QObject *parent = 0);
public:

    static QObject *initializeEngine(QQmlEngine *engine);
    static ThemeEngine *instance();

    // getter/setters
    QString error() const;
    void resetError();
    QString theme() const;

Q_SIGNALS:
    void errorChanged();
    void themeChanged();

public Q_SLOTS:
    bool loadTheme(const QUrl &themeFile);

private:
    Q_DISABLE_COPY(ThemeEngine)
    Q_DECLARE_PRIVATE(ThemeEngine)
    QScopedPointer<ThemeEnginePrivate> d_ptr;

#ifdef QT_TESTLIB_LIB
    friend class tst_ThemeEnginePrivate;
#endif

    Q_PRIVATE_SLOT(d_func(), void _q_updateTheme())
    Q_PRIVATE_SLOT(d_func(), void _q_reloadTheme())
};

#endif // THEMEENGINE_H
