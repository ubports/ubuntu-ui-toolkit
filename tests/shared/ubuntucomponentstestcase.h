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
 */

#ifndef UBUNTUCOMPONENTSTESTCASE_H
#define UBUNTUCOMPONENTSTESTCASE_H

#include <QtTest>
#include <QtDeclarative>
#include <QtCore>

class UbuntuComponentsTestCase : public QObject
{
    Q_OBJECT

protected:
    void loadAndCreateComponentObject();
    void checkPropertyExistsAndExpectedType(const QString& name, const QVariant::Type& expectedType);
    void checkPropertyValue(const QString& name, const QVariant& expectedValue);
    void setPropertyValue(const QString& name, const QVariant& value);
    void checkSimpleSignal(const char* signalName, const char* member);

    void setComponentFileName(const QString& fileName) { m_sComponentFileName = fileName; }
    QString componentFileName() { return m_sComponentFileName; }
    QObject* componentObject() { return m_pComponentObject; }

    QObject* m_pComponentObject;
    QString m_sComponentFileName;
};

#endif // UBUNTUCOMPONENTSTESTCASE_H
