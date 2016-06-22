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
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#include "statesaverbackend_p.h"
#include "ucapplication.h"
#include <QtQml/QQmlContext>
#include <QtQml/QQmlProperty>
#include <QtQml/qqmlinfo.h>
#include <QtQml/qqml.h>
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QStringList>
#include "i18n.h"
#include "quickutils.h"
#include <QtCore/QStandardPaths>

#include "unixsignalhandler_p.h"

UT_NAMESPACE_BEGIN

StateSaverBackend *StateSaverBackend::m_instance = nullptr;

StateSaverBackend::StateSaverBackend(QObject *parent)
    : QObject(parent)
    , m_archive(0)
    , m_globalEnabled(true)
{
    // connect to application quit signal so when that is called, we can clean the states saved
    QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
                     this, &StateSaverBackend::cleanup);
    QObject::connect(QuickUtils::instance(), &QuickUtils::activated,
                     this, &StateSaverBackend::reset);
    QObject::connect(QuickUtils::instance(), &QuickUtils::deactivated,
                     this, &StateSaverBackend::initiateStateSaving);
    // catch eventual app name changes so we can have different path for the states if needed
    QObject::connect(UCApplication::instance(), &UCApplication::applicationNameChanged,
                     this, &StateSaverBackend::initialize);
    if (!UCApplication::instance()->applicationName().isEmpty()) {
        initialize();
    }

    UnixSignalHandler::instance().connectSignal(UnixSignalHandler::Terminate);
    UnixSignalHandler::instance().connectSignal(UnixSignalHandler::Interrupt);
    QObject::connect(&UnixSignalHandler::instance(), SIGNAL(signalTriggered(int)),
                     this, SLOT(signalHandler(int)));
}

StateSaverBackend::~StateSaverBackend()
{
    if (m_archive) {
        delete m_archive;
    }
    m_instance = nullptr;
}

void StateSaverBackend::initialize()
{
    if (m_archive) {
        // delete previous archive
        QFile archiveFile(m_archive.data()->fileName());
        archiveFile.remove();
        delete m_archive.data();
        m_archive.clear();
    }
    QString applicationName(UCApplication::instance()->applicationName());
    if (applicationName.isEmpty()) {
        qCritical() << "[StateSaver] Cannot create appstate file, application name not defined.";
        return;
    }
    // make sure the path is in sync with https://wiki.ubuntu.com/SecurityTeam/Specifications/ApplicationConfinement
    // the file must be saved under XDG_RUNTIME_DIR/<APP_PKGNAME> path.
    // NOTE!!: we cannot use QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation)
    // as that is going to perform a chmod +w on the path, see bug #1359831. Therefore we must
    // fetch the XDG_RUNTIME_DIR either from QStandardPaths::standardLocations() or from env var
    // see bug https://bugreports.qt-project.org/browse/QTBUG-41735
    QString runtimeDir = QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation);
    if (runtimeDir.isEmpty()) {
        runtimeDir = qgetenv("XDG_RUNTIME_DIR");
    }
    if (runtimeDir.isEmpty()) {
        qCritical() << "[StateSaver] No XDG_RUNTIME_DIR path set, cannot create appstate file.";
        return;
    }
    m_archive = new QSettings(QString("%1/%2/statesaver.appstate").
                              arg(runtimeDir).
                              arg(applicationName), QSettings::NativeFormat);
    m_archive->setFallbacksEnabled(false);
}

void StateSaverBackend::cleanup()
{
    reset();
    m_archive.clear();
}

void StateSaverBackend::signalHandler(int type)
{
    if (type == UnixSignalHandler::Interrupt) {
        Q_EMIT initiateStateSaving();
        // disconnect aboutToQuit() so the state file doesn't get wiped upon quit
        QObject::disconnect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
                         this, &StateSaverBackend::cleanup);
    }
    QCoreApplication::quit();
}

bool StateSaverBackend::enabled() const
{
    return m_globalEnabled;
}
void StateSaverBackend::setEnabled(bool enabled)
{
    if (m_globalEnabled != enabled) {
        m_globalEnabled = enabled;
        Q_EMIT enabledChanged(m_globalEnabled);
        if (!m_globalEnabled) {
            reset();
        }
    }
}

bool StateSaverBackend::registerId(const QString &id)
{
    if (m_register.contains(id)) {
        return false;
    }
    m_register.insert(id);
    return true;
}

void StateSaverBackend::removeId(const QString &id)
{
    m_register.remove(id);
}

int StateSaverBackend::load(const QString &id, QObject *item, const QStringList &properties)
{
    if (m_archive.isNull()) {
        return 0;
    }

    int result = 0;
    // save the previous group
    bool restorePreviousGroup = !m_archive->group().isEmpty();
    if (restorePreviousGroup) {
        m_groupStack.push(m_archive->group());
        // leave the group so we can read the next one
        m_archive->endGroup();
    }
    m_archive.data()->beginGroup(id);
    QStringList propertyNames = m_archive.data()->childKeys();
    Q_FOREACH(const QString &propertyName, propertyNames) {
        QVariant value = m_archive.data()->value(propertyName);
        if (!properties.contains(propertyName)) {
            // skip the property
            continue;
        }
        QQmlProperty qmlProperty(item, propertyName.toLocal8Bit().constData(), qmlContext(item));
        if (qmlProperty.isValid() && qmlProperty.isWritable()) {
            QVariant type = m_archive.data()->value(propertyName + "_TYPE");
            value.convert(type.toInt());
            bool writeSuccess = qmlProperty.write(value);
            if (writeSuccess) {
                result++;
            } else {
                qmlInfo(item) << QStringLiteral("property \"%1\" of "
                    "object %2 has type %3 and cannot be set to value \"%4\" of"
                    " type %5").arg(propertyName)
                               .arg(qmlContext(item)->nameForObject(item))
                               .arg(qmlProperty.propertyTypeName())
                               .arg(value.toString())
                               .arg(value.typeName());
            }
        } else {
            qmlInfo(item) << QStringLiteral("property \"%1\" does not exist or is not writable for object %2")
                             .arg(propertyName).arg(qmlContext(item)->nameForObject(item));
        }
    }
    // drop cache once properties are successfully restored
    m_archive.data()->remove("");
    m_archive.data()->endGroup();
    // restore leaved group if needed
    if (restorePreviousGroup) {
        m_archive->beginGroup(m_groupStack.pop());
    }
    return result;
}

int StateSaverBackend::save(const QString &id, QObject *item, const QStringList &properties)
{
    if (m_archive.isNull()) {
        return 0;
    }
    m_archive.data()->beginGroup(id);
    int result = 0;
    Q_FOREACH(const QString &propertyName, properties) {
        QQmlProperty qmlProperty(item, propertyName.toLocal8Bit().constData());
        if (qmlProperty.isValid()) {
            QVariant value = qmlProperty.read();
            if (static_cast<QMetaType::Type>(value.type()) != QMetaType::QObjectStar) {
                if (value.userType() == qMetaTypeId<QJSValue>()) {
                    value = value.value<QJSValue>().toVariant();
                }
                m_archive.data()->setValue(propertyName, value);
                /* Save the type of the property along with its value.
                 * This is important because QSettings deserializes values as QString.
                 * Setting these strings to QML properties usually works because the
                 * implicit type conversion from string to the type of the QML property
                 * usually works. In some cases cases however (e.g. enum) it fails.
                 *
                 * See Qt Bug: https://bugreports.qt-project.org/browse/QTBUG-40474
                 */
                m_archive.data()->setValue(propertyName + "_TYPE", QVariant::fromValue((int)value.type()));
                result++;
            }
        }
    }
    m_archive.data()->endGroup();
    m_archive.data()->sync();
    return result;
}

/*
 * The method resets the register and the state archive for the application.
 */
bool StateSaverBackend::reset()
{
    m_register.clear();
    if (m_archive) {
        QFile archiveFile(m_archive.data()->fileName());
        return archiveFile.remove();
    }
    return true;
}

UT_NAMESPACE_END
