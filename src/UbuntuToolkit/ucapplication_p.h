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

#ifndef UCAPPLICATION_P_H
#define UCAPPLICATION_P_H

#include <QtCore/QObject>

#include <UbuntuToolkit/ubuntutoolkitglobal.h>

class QQmlContext;
class QQmlEngine;

UT_NAMESPACE_BEGIN

class UBUNTUTOOLKIT_EXPORT UCApplication : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString applicationName READ applicationName WRITE setApplicationName NOTIFY applicationNameChanged)
    Q_PROPERTY(QObject* inputMethod READ inputMethod WRITE setInputMethod NOTIFY inputMethodChanged)
    Q_PROPERTY(Qt::LayoutDirection layoutDirection READ layoutDirection WRITE setLayoutDirection NOTIFY layoutDirectionChanged)

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
    Qt::LayoutDirection layoutDirection();

    // setter
    void setContext(QQmlContext* context);
    void setApplicationName(const QString& applicationName);
    void setInputMethod(QObject* inputMethod);
    void setLayoutDirection(Qt::LayoutDirection layoutDirection);

private:
    QQmlContext* m_context;
    QObject* m_inputMethod;
    static UCApplication *m_app;

Q_SIGNALS:
    void applicationNameChanged();
    void inputMethodChanged();
    void layoutDirectionChanged();
};

UT_NAMESPACE_END

#endif // UCAPPLICATION_P_H
