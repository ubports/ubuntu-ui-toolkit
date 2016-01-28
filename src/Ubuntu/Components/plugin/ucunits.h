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
#include <QtQml/QQmlParserStatus>
#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QUrl>

class QWindow;
class QQuickItem;
class QPlatformWindow;

class UCUnits;
static QHash<QWindow*, UCUnits*> _q_unitsHash;


class UCUnits : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(float gridUnit READ gridUnit WRITE setGridUnit NOTIFY gridUnitChanged)

public:
    static UCUnits& instance() {
        static UCUnits instance;
        return instance;
    }
    static UCUnits& instance(QQuickItem *item); // returns unique instance per QWindow

    UCUnits(QObject *parent = 0);
    virtual ~UCUnits();

    Q_INVOKABLE float dp(float value);
    Q_INVOKABLE float gu(float value);
    QString resolveResource(const QUrl& url);

    // getters
    float gridUnit();

    // setters
    void setGridUnit(float gridUnit);

    // from QQmlParserStatus
    void classBegin() Q_DECL_OVERRIDE;
    void componentComplete() Q_DECL_OVERRIDE {}

Q_SIGNALS:
    void gridUnitChanged();

protected:
    QString suffixForGridUnit(float gridUnit);
    float gridUnitSuffixFromFileName(const QString &fileName);

private Q_SLOTS:
    void windowPropertyChanged(QPlatformWindow *window, const QString &propertyName);

private:
    float m_devicePixelRatio;
    float m_gridUnit;
};

#endif // UBUNTU_COMPONENTS_UNITS_H
