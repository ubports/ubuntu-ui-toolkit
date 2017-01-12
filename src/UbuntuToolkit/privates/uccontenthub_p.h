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

#ifndef UCCONTENTHUB_P_H
#define UCCONTENTHUB_P_H

#include <QtCore/QObject>
#include <QtCore/QMimeData>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusConnection>

#include <UbuntuToolkit/ubuntutoolkitglobal.h>

UT_NAMESPACE_BEGIN

class UBUNTUTOOLKIT_EXPORT UCContentHub : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool canPaste READ canPaste NOTIFY canPasteChanged)

public:
    UCContentHub(QObject* parent = 0);
    ~UCContentHub();

    Q_INVOKABLE void requestPaste();

    bool canPaste();

Q_SIGNALS:
    void pasteSelected(const QString &data);
    void canPasteChanged();

private Q_SLOTS:
    void onPasteSelected(QString appId, QByteArray mimedata, bool pasteAsRichText);
    void onPasteboardChanged();

private:
    QString getAppProfile();
    QMimeData* deserializeMimeData(const QByteArray &serializedMimeData);
    bool checkPasteFormats();

    QDBusInterface *m_dbusIface;
    QDBusInterface *m_contentHubIface;

    bool m_canPaste;
};

UT_NAMESPACE_END

#endif  // UCCONTENTHUB_P_H
