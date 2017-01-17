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

Q_LOGGING_CATEGORY(ucContentHub, "ubuntu.components.UCContentHub", QtMsgType::QtWarningMsg)

#define CONTENT_HUB_TRACE(params) qCDebug(ucContentHub) << params

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
      m_canPaste(false),
      m_targetItem(0)
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
            SLOT(onPasteSelected(QString, QByteArray, bool))
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

void UCContentHub::requestPaste(QQuickItem *targetItem)
{
    if (!m_contentHubIface->isValid()) {
        CONTENT_HUB_TRACE("Invalid Content Hub DBusInterface");
        return;
    }

    m_targetItem = targetItem;

    QString appProfile = getAppProfile();
    CONTENT_HUB_TRACE("AppArmor profile:" << appProfile);

    m_contentHubIface->call(QStringLiteral("RequestPasteByAppId"), appProfile);
}

bool UCContentHub::canPaste()
{
    return m_canPaste;
}

void UCContentHub::onPasteSelected(QString appId, QByteArray mimedata, bool pasteAsRichText)
{
    if (getAppProfile() != appId) {
        return;
    }

    if (mimedata.isNull()) {
        CONTENT_HUB_TRACE("onPasteSelected: Invalid MimeData received");
        return;
    }

    QMimeData* deserialized = deserializeMimeData(mimedata);
    if (deserialized->hasImage()) {
        if (deserialized->imageData().toByteArray().isEmpty()) {
            Q_EMIT pasteSelected(m_targetItem, deserialized->html());
        } else {
            Q_EMIT pasteSelected(m_targetItem, deserialized->imageData().toByteArray());
        }
    } else if (deserialized->hasHtml() && pasteAsRichText) {
        Q_EMIT pasteSelected(m_targetItem, deserialized->html());
    } else {
        Q_EMIT pasteSelected(m_targetItem, deserialized->text());
    }
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
        CONTENT_HUB_TRACE("Invalid DBus DBusInterface");
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
        CONTENT_HUB_TRACE("Invalid Content Hub DBusInterface");
        return false;
    }

    QDBusReply<QStringList> reply = m_contentHubIface->call(QStringLiteral("PasteFormats"));
    if (reply.isValid()) {
        // TODO: ContentHub clipboard keeps a list of all available paste formats.
        // Probably apps could make use of this information to check if a specific
        // data type is available, instead of only checking if list is empty or not.
        // (LP: #1657111)
        return !reply.value().isEmpty();
    } else {
        CONTENT_HUB_TRACE("Invalid return from DBus call PasteFormats");
    }

    return false;
}

UT_NAMESPACE_END
