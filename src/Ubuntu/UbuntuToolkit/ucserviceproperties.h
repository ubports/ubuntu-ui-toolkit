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

#ifndef UCSERVICEPROPERTIES_H
#define UCSERVICEPROPERTIES_H

#include <QtCore/QObject>
#include <QtQml/QQmlParserStatus>
#include <ubuntutoolkitglobal.h>

namespace UbuntuToolkit {

class UCServicePropertiesPrivate;
class UBUNTUTOOLKIT_EXPORT UCServiceProperties : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(ServiceType type READ type WRITE setType NOTIFY typeChanged REVISION 1)
    Q_PROPERTY(QString service READ service WRITE setService NOTIFY serviceChanged REVISION 1)
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged REVISION 1)
    Q_PROPERTY(QString serviceInterface READ interface WRITE setInterface NOTIFY serviceInterfaceChanged REVISION 1)
    Q_PROPERTY(QString adaptorInterface READ adaptor WRITE setAdaptor NOTIFY adaptorInterfaceChanged REVISION 1)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged REVISION 1)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged REVISION 1)

    Q_ENUMS(ServiceType Status)
public:
    enum ServiceType {
        Undefined,
        System,
        Session
    };
    enum Status {
        Inactive,
        ConnectionError,
        Synchronizing,
        Active
    };

    explicit UCServiceProperties(QObject *parent = 0);
    ~UCServiceProperties();
    void classBegin() override;
    void componentComplete() override;

    ServiceType type() const;
    void setType(ServiceType type);
    QString service() const;
    void setService(const QString &value);
    QString path() const;
    void setPath(const QString &value);
    QString interface() const;
    void setInterface(const QString &value);
    QString adaptor() const;
    void setAdaptor(const QString &value);
    QString error();
    Status status() const;

Q_SIGNALS:
    void typeChanged();
    void serviceChanged();
    void pathChanged();
    void serviceInterfaceChanged();
    void adaptorInterfaceChanged();
    void errorChanged();
    void statusChanged();

private:
    UCServicePropertiesPrivate *d_ptr;
    Q_DECLARE_PRIVATE_D(d_ptr, UCServiceProperties)
};

UT_NAMESPACE_END

#endif // UCSERVICEPROPERTIES_H
