/*
 * Copyright 2014 Canonical Ltd.
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
 */

#ifndef UCSERVICEPROPERTIES_P_H
#define UCSERVICEPROPERTIES_P_H

#include "ucserviceproperties.h"
#include <QtCore/QStringList>

class UCServicePropertiesPrivate
{
public:
    UCServicePropertiesPrivate(UCServiceProperties *qq);
    virtual ~UCServicePropertiesPrivate();

    static UCServicePropertiesPrivate *get(UCServiceProperties *service);
    void warning(const QString &message);
    void setError(const QString &msg);
    void setStatus(UCServiceProperties::Status status);

    virtual bool init() = 0;
    virtual bool fetchPropertyValues() = 0;
    virtual bool readProperty(const QString &property) = 0;
    virtual bool testProperty(const QString &property, const QVariant &value) = 0;

    // data
    UCServiceProperties *q_ptr;
    bool ready:1;
    UCServiceProperties::Status status;
    UCServiceProperties::ServiceType type;
    QString service;
    QString path;
    QString interface;
    QString adaptor;
    QString error;
    QStringList properties;
};

UCServicePropertiesPrivate *createServicePropertiesAdapter(UCServiceProperties *owner);

#endif // UCSERVICEPROPERTIES_P_H
