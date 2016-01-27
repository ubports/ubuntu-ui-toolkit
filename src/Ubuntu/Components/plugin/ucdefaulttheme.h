/*
 * Copyright 2013-2015 Canonical Ltd.
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
 * Authors: Zsombor Egri <zsombor.egri@canonical.com>
 *          Florian Boucault <florian.boucault@canonical.com>
 */

#ifndef UCDEFAULTTHEME_H
#define UCDEFAULTTHEME_H

#include <QtCore/QFileSystemWatcher>
#include <QtCore/QSettings>
#include <QtCore/QObject>

class UCDefaultTheme : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString themeName READ themeName WRITE setThemeName NOTIFY themeNameChanged)
public:
    explicit UCDefaultTheme(QObject *parent = 0);

    // getter/setters
    QString themeName() const;
    void setThemeName(const QString &themeName);

Q_SIGNALS:
    void themeNameChanged();

private Q_SLOTS:
    void reloadSettings();
private:
    QFileSystemWatcher m_settingsFileWatcher;
    QSettings m_settings;
    QString m_themeName;
};

#endif // UCDEFAULTTHEME_H
