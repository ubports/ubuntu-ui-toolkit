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

#include <QObject>
#include <QUrl>
#include <qdeclarative.h>

class StyleRule;
class QDeclarativeEngine;
class StyledItem;

class ThemeEnginePrivate;
class ThemeEngine : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString error READ error RESET resetError NOTIFY errorChanged)
    Q_PROPERTY(QString currentTheme READ currentTheme NOTIFY themeChanged)
public:
    ThemeEngine(QObject *parent = 0);
    ~ThemeEngine();

    static bool initialize(QDeclarativeEngine *engine);
    static ThemeEngine *instance();

    bool registerInstanceId(StyledItem *item, const QString &newId);
    StyleRule *lookupStyleRule(StyledItem *item, bool forceClassName = false);

    // getter/setters
    QString error() const;
    void resetError();
    QString currentTheme() const;

signals:
    void errorChanged();
    void themeChanged();

public slots:
    bool loadTheme(const QUrl &themeFile);

    bool setTheme(const QString &theme, bool global = false);

private:
    Q_DISABLE_COPY(ThemeEngine)
    Q_DECLARE_PRIVATE(ThemeEngine)
    QScopedPointer<ThemeEnginePrivate> d_ptr;

#ifdef QT_TESTLIB_LIB
    friend class ThemeEnginePrivateTest;
#endif

    Q_PRIVATE_SLOT(d_func(), void _q_continueThemeLoading())
    Q_PRIVATE_SLOT(d_func(), void _q_updateTheme())
};

QML_DECLARE_TYPE(ThemeEngine)

#endif // THEMEENGINE_H
