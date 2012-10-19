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

#ifndef THEMESETTINGS_P_H
#define THEMESETTINGS_P_H

#include <QtCore/QFileSystemWatcher>
#include <QtCore/QSettings>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QObject>


class ThemeSettings : public QObject
{
    Q_OBJECT
public:
    ThemeSettings(QObject *parent = 0);
    QUrl themeFile() const;
    QUrl setTheme(const QString &theme, bool global);
    QStringList imports() const;

Q_SIGNALS:
    void themeSettingsChanged();

private Q_SLOTS:
    void refreshSettings();
private:
    QFileSystemWatcher configWatcher;
    QSettings globalSettings;
    QSettings appSettings;
    bool hasAppSettings;
};

#endif // THEMESETTINGS_P_H
