/*
 * Copyright 2016 Canonical Ltd.
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

#ifndef ENGINEDATA_H
#define ENGINEDATA_H

#include <QtCore/QObject>
#include <ubuntutoolkitglobal.h>

class QQmlEngine;

UT_NAMESPACE_BEGIN

class UBUNTUTOOLKIT_EXPORT EngineData : public QObject
{
    Q_OBJECT
public:
    explicit EngineData(QObject *parent = 0);

    static bool isDeclared(QQmlEngine *engine);
    static void create(QQmlEngine *engine, const QUrl &urlOverride);

    static QUrl baseUrl(QQmlEngine *engine);

private:
    QUrl m_baseUrl;
};

UT_NAMESPACE_END

#endif // ENGINEDATA_H
