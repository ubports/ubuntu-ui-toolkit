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
 * Author: Florian Boucault <florian.boucault@canonical.com>
 */

#ifndef UBUNTU_COMPONENTS_UNITS_H
#define UBUNTU_COMPONENTS_UNITS_H

#include <QObject>
#include <QtCore/QHash>
#include <QtCore/QUrl>

#define RESOURCES_UNIT_FILE "resources_unit"

class UCUnits : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float gridUnit READ gridUnit WRITE setGridUnit NOTIFY gridUnitChanged)

public:
    static UCUnits& instance() {
        static UCUnits instance;
        return instance;
    }

    explicit UCUnits(QObject *parent = 0);
    bool loadResourcesUnitFile(const QUrl& baseUrl, QString fileName);
    Q_INVOKABLE float dp(float value);
    Q_INVOKABLE float gu(float value);
    QString resolveResource(const QUrl& url);

    // getters
    float gridUnit();

    // setters
    void setGridUnit(float gridUnit);

Q_SIGNALS:
    void gridUnitChanged();

protected:
    QString suffixForGridUnit(float gridUnit);

private:
    float m_gridUnit;
    float m_resourcesUnit;
};

#endif // UBUNTU_COMPONENTS_UNITS_H
