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
 * Author: Arthur Mello <arthur.mello@canonical.com>
 */

#include "privates/uccontenthub_p.h"

#include <QtCore/QLoggingCategory>
#include <QtDBus/QDBusInterface>

Q_LOGGING_CATEGORY(ucContentHub, "ubuntu.components.PrivateContentHub", QtMsgType::QtWarningMsg)

#define CONTHUB_TRACE(params) qCDebug(ucContentHub) << params

static const QString contentHubService = QStringLiteral("com.ubuntu.content.dbus.Service");
static const QString contentHubObjectPath = QStringLiteral("/");
static const QString contentHubInterface = QStringLiteral("com.ubuntu.content.dbus.Service");

static const QString dbusService = QStringLiteral("org.freedesktop.DBus");
static const QString dbusObjectPath = QStringLiteral("/org/freedesktop/DBus");
static const QString dbusInterface = QStringLiteral("org.freedesktop.DBus");

UT_NAMESPACE_BEGIN

UCContentHub::UCContentHub(QObject *parent)
    : QObject(parent),
      m_dbusIface(0),
      m_contentHubIface(0),
      m_canPaste(false)
{
    m_dbusIface = new QDBusInterface(dbusService,
                                     dbusObjectPath,
                                     dbusInterface,
                                     QDBusConnection::sessionBus(),
                                     this);

    m_contentHubIface = new QDBusInterface(contentHubService,
                                           contentHubObjectPath,
                                           contentHubInterface,
                                           QDBusConnection::sessionBus(),
                                           this);
    if (m_contentHubIface->isValid()) {
        m_contentHubIface->connection().connect(
            contentHubService,
            contentHubObjectPath,
            contentHubInterface,
            QStringLiteral("PasteSelected"),
            this,
            SLOT(onPasteSelected(QString, QByteArray))
        );

        m_contentHubIface->connection().connect(
            contentHubService,
            contentHubObjectPath,
            contentHubInterface,
            QStringLiteral("PasteboardChanged"),
            this,
            SLOT(onPasteboardChanged())
        );

        m_canPaste = checkPasteFormats();
    }
}

UCContentHub::~UCContentHub()
{
    if (m_dbusIface) {
        delete m_dbusIface;
    }

    if (m_contentHubIface) {
        delete m_contentHubIface;
    }
}

void UCContentHub::requestPaste()
{
    if (!m_contentHubIface->isValid()) {
        CONTHUB_TRACE("Invalid Content Hub DBusInterface");
        return;
    }

    QString appProfile = getAppProfile();
    qDebug() << "[UITK ContentHub] AppArmor profile:" << appProfile;

    m_contentHubIface->call(QStringLiteral("RequestPasteByAppId"), appProfile);
}

bool UCContentHub::canPaste()
{
    return m_canPaste;
}

void UCContentHub::onPasteSelected(QString appId, QByteArray mimedata)
{
    if (getAppProfile() != appId) {
        return;
    }

    if (mimedata.isNull()) {
        CONTHUB_TRACE("onPasteSelected: Invalid MimeData received");
        return;
    }

    QMimeData* deserialized = deserializeMimeData(mimedata);
    Q_EMIT pasteSelected(deserialized->text());
}

void UCContentHub::onPasteboardChanged()
{
    if (checkPasteFormats() != m_canPaste) {
        m_canPaste = !m_canPaste;
        Q_EMIT canPasteChanged();
    }
}

QString UCContentHub::getAppProfile()
{
    if (!m_dbusIface->isValid()) {
        CONTHUB_TRACE("Invalid DBus DBusInterface");
        return QString();
    }

    QDBusReply<QString> reply = m_dbusIface->call("GetConnectionAppArmorSecurityContext", QDBusConnection::sessionBus().baseService());
    if (reply.isValid()) {
        return reply.value();
    }

    return QString();
}

QMimeData* UCContentHub::deserializeMimeData(const QByteArray &serializedMimeData)
{
    int maxFormatsCount = 16;

    if (static_cast<std::size_t>(serializedMimeData.size()) < sizeof(int)) {
        // Data is invalid
        return nullptr;
    }

    QMimeData *mimeData = new QMimeData;

    const char* const buffer = serializedMimeData.constData();
    const int* const header = reinterpret_cast<const int*>(serializedMimeData.constData());

    const int count = qMin(header[0], maxFormatsCount);

    for (int i = 0; i < count; i++) {
        const int formatOffset = header[i*4+1];
        const int formatSize = header[i*4+2];
        const int dataOffset = header[i*4+3];
        const int dataSize = header[i*4+4];

        if (formatOffset + formatSize <= serializedMimeData.size()
            && dataOffset + dataSize <= serializedMimeData.size()) {

            QString mimeType = QString::fromLatin1(&buffer[formatOffset], formatSize);
            QByteArray mimeDataBytes(&buffer[dataOffset], dataSize);

            mimeData->setData(mimeType, mimeDataBytes);
        }
    }

    return mimeData;
}

bool UCContentHub::checkPasteFormats()
{
    if (!m_contentHubIface->isValid()) {
        CONTHUB_TRACE("Invalid Content Hub DBusInterface");
        return false;
    }

    QDBusReply<QStringList> reply = m_contentHubIface->call(QStringLiteral("PasteFormats"));
    if (reply.isValid()) {
        // TODO: In  the future check if Paste Formats list has an interesting format
        return !reply.value().isEmpty();
    } else {
        CONTHUB_TRACE("Invalid return from DBus call PasteFormats");
    }

    return false;
}

UT_NAMESPACE_END
