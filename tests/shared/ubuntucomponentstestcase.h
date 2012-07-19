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

class UbuntuComponentsTestCase : public QObject
{
    Q_OBJECT

public:

    enum FailureOnInit {
        NO_ERRORS = 0,
        FAILED_TO_LOAD_FROM_FILE,
        FAILED_TO_CREATE_COMPONENT,
        FAILED_FILE_DOES_NOT_EXIST
    };


protected:
    bool checkPropertyAndExpectedType(const QString& name, QVariant::Type expectedType) {
        QMetaProperty property = m_pComponentObject->metaObject()->property(m_pComponentObject->metaObject()->indexOfProperty(name.toLatin1().data()));
        if (!property.isValid()) {
            qWarning() << "Property does not exist";
            return false;
        }
        if (property.type()!=expectedType) {
            qWarning() << "Expected property type was" << expectedType << "but" << property.type() << "was found.";
            return false;
        }
        return true;
    }


    int loadAndCreateComponentObject() {
        m_sErrorString = "";
        QDeclarativeView* tmpView = new QDeclarativeView;
        tmpView->engine()->addImportPath("../../../");
        QDeclarativeComponent tmpComponent(tmpView->engine());
        if (!QFile(m_sComponentFileName).exists()) {
            m_sErrorString = QString("File does not exist %0").arg(m_sComponentFileName);
            qWarning() << m_sErrorString;
            qDebug() << "Current path is" << QApplication::applicationDirPath();
            return FAILED_FILE_DOES_NOT_EXIST;
        }
        QFile componentFile(m_sComponentFileName);
        if (!componentFile.open(QIODevice::ReadOnly)) {
            m_sErrorString = "Failed to load from file";
            qWarning() << m_sErrorString;
            return FAILED_TO_LOAD_FROM_FILE;
        }
        tmpComponent.setData(componentFile.readAll(),QUrl());
        m_pComponentObject = tmpComponent.create();
        QObject::connect(qApp,SIGNAL(aboutToQuit()),tmpView,SLOT(deleteLater()));
        if (!m_pComponentObject) {
            qWarning() << "Failed to create component";
            qDebug() << tmpComponent.errorString();
            m_sErrorString = tmpComponent.errorString();
            return FAILED_TO_CREATE_COMPONENT;
        }
        return NO_ERRORS;
    }


    QString errorString() { return m_sErrorString; }
    void setComponentFileName(QString fileName) { m_sComponentFileName = fileName; }
    QString componentFileName() { return m_sComponentFileName; }
    QObject* componentObject() { return m_pComponentObject; }

    QObject* m_pComponentObject;
    QString m_sComponentFileName;
    QString m_sErrorString;
};

#endif // UBUNTUCOMPONENTSTESTCASE_H
