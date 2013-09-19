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
#include <QtQml/QQmlProperty>
#include <QtQml>
#include <QtCore/QCoreApplication>
#include "i18n.h"

StateSaverBackend::StateSaverBackend(QObject *parent)
    : QObject(parent)
    , m_archive(0)
{
    if (!UCApplication::instance().applicationName().isEmpty()) {
        initialize();
    } else {
        qDebug() << "PENDING ARCHIVE";
        QObject::connect(&UCApplication::instance(), &UCApplication::applicationNameChanged,
                         this, &StateSaverBackend::initialize);
    }

    QCoreApplication::instance()->installEventFilter(this);

    // idle timer, set to 3 seconds
    QObject::connect(&m_timer, &QTimer::timeout, this, &StateSaverBackend::idle);
    m_timer.setInterval(3000);
    m_timer.start();
    m_timer.setSingleShot(true);
}

StateSaverBackend::~StateSaverBackend()
{
    if (m_archive) {
        delete m_archive;
    }
}

bool StateSaverBackend::eventFilter(QObject *object, QEvent *event)
{
    switch (event->type()) {
    case QEvent::ApplicationDeactivate: {
        qDebug() << "App deactivated";
        Q_EMIT deactivated();
        return true;
    }
    case QEvent::KeyRelease:
    case QEvent::MouseMove:
    case QEvent::TouchUpdate:
        // restart idle timer when one of these events occurr
        m_timer.stop();
        m_timer.start();
        return true;
    default:
        return QObject::eventFilter(object, event);
    }
}

void StateSaverBackend::resetRegister()
{
    m_register.clear();
}

void StateSaverBackend::initialize()
{
    // FIXME: replace with default constructed setting once the organization name gets fixed
    m_archive = new QSettings(UCApplication::instance().applicationName());
    m_archive->setFallbacksEnabled(false);
    qDebug() << "SETTINGS" << m_archive.data()->fileName() << UCApplication::instance().applicationName();
}

bool StateSaverBackend::registerId(const QString &id)
{
    StateSaverBackend &saver = instance();
    if (saver.m_register.contains(id)) {
        return false;
    }
    saver.m_register.insert(id);
    return true;
}

void StateSaverBackend::removeId(const QString &id)
{
    instance().m_register.remove(id);
}

int StateSaverBackend::load(const QString &id, QObject *item, const QStringList &properties)
{
    if (m_archive.isNull()) {
        return 0;
    }

    int result = 0;
    m_archive.data()->beginGroup(id);
    QStringList propertyNames = m_archive.data()->childKeys();
    Q_FOREACH(QString propertyName, propertyNames) {
        QVariant value = m_archive.data()->value(propertyName);
        qDebug() << "restoring" << id << propertyName << value;
        if (!properties.contains(propertyName)) {
            // skip the property
            qDebug() << "skip" << id << propertyName;
            continue;
        }
        QQmlProperty qmlProperty(item, propertyName.toLocal8Bit().constData());
        if (qmlProperty.isValid() && qmlProperty.isWritable()) {
            qmlProperty.write(value);
            result++;
        } else {
            qmlInfo(item) << UbuntuI18n::instance().tr("property \"%1\" does not exist or is not writable for object %2")
                             .arg(propertyName).arg(qmlContext(item)->nameForObject(item));
        }
    }
    m_archive.data()->endGroup();
    return result;
}

int StateSaverBackend::save(const QString &id, QObject *item, const QStringList &properties)
{
    if (m_archive.isNull()) {
        return 0;
    }
    m_archive.data()->beginGroup(id);

    Q_FOREACH(QString propertyName, properties) {
        QQmlProperty qmlProperty(item, propertyName.toLocal8Bit().constData());
        QVariant value = qmlProperty.read();
        qDebug() << "saving" << propertyName << value;
        m_archive.data()->setValue(propertyName, value);
    }

    m_archive.data()->endGroup();
    m_archive.data()->sync();
    return properties.count();
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


