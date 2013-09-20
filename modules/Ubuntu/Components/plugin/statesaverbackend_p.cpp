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
        QObject::connect(&UCApplication::instance(), &UCApplication::applicationNameChanged,
                         this, &StateSaverBackend::initialize);
    }

    QCoreApplication::instance()->installEventFilter(this);
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
        Q_EMIT forcedSave();
        return true;
    }
    default:
        return QObject::eventFilter(object, event);
    }
}

void StateSaverBackend::initialize()
{
    m_archive = new QSettings(UCApplication::instance().applicationName());
    m_archive->setFallbacksEnabled(false);
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
    m_archive.data()->beginGroup(id);
    QStringList propertyNames = m_archive.data()->childKeys();
    Q_FOREACH(const QString &propertyName, propertyNames) {
        QVariant value = m_archive.data()->value(propertyName);
        if (!properties.contains(propertyName)) {
            // skip the property
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
    int result = 0;
    Q_FOREACH(const QString &propertyName, properties) {
        QQmlProperty qmlProperty(item, propertyName.toLocal8Bit().constData());
        if (qmlProperty.isValid()) {
            QVariant value = qmlProperty.read();
            if (static_cast<QMetaType::Type>(value.type()) != QMetaType::QObjectStar) {
                m_archive.data()->setValue(propertyName, value);
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


