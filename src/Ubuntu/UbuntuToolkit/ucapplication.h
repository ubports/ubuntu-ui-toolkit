/*
 * Copyright 2013 Canonical Ltd.
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
 * Author: Christian Dywan <christian.dywan@canonical.om>
 */

#ifndef UBUNTU_COMPONENTS_APPLICATION_H
#define UBUNTU_COMPONENTS_APPLICATION_H

#include <QtCore/QObject>
#include "ubuntutoolkitglobal.h"

class QQmlContext;
class QQmlEngine;

UT_NAMESPACE_BEGIN

class UBUNTUTOOLKIT_EXPORT UCApplication : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString applicationName READ applicationName WRITE setApplicationName NOTIFY applicationNameChanged)
    Q_PROPERTY(QObject* inputMethod READ inputMethod WRITE setInputMethod NOTIFY inputMethodChanged)

private:
    Q_DISABLE_COPY(UCApplication)
    explicit UCApplication(QObject* parent = 0);
    ~UCApplication();

public:
    static UCApplication *instance(QObject *parent = Q_NULLPTR) {
        if (!m_app) {
            if (!parent) {
                qFatal("Creating UbuntuApplication singleton requires a parent object!");
            }
            m_app = new UCApplication(parent);
        }
        return m_app;
    }

    // getter
    QString applicationName();
    QObject* inputMethod();

    // setter
    void setContext(QQmlContext* context);
    void setApplicationName(const QString& applicationName);
    void setInputMethod(QObject* inputMethod);

private:
    QQmlContext* m_context;
    QObject* m_inputMethod;
    static UCApplication *m_app;

Q_SIGNALS:
    void applicationNameChanged();
    void inputMethodChanged();
};

UT_NAMESPACE_END

#endif // UBUNTU_COMPONENTS_I18N_H
