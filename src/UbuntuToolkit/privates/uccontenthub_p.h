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

#include <UbuntuToolkit/ubuntutoolkitglobal.h>

class QMimeData;
class QDBusInterface;
class QDBusServiceWatcher;
class QQuickItem;

UT_NAMESPACE_BEGIN

class UBUNTUTOOLKIT_EXPORT UCContentHub : public QObject
{
    Q_OBJECT
    friend class tst_UCContentHub;
    Q_PROPERTY(bool canPaste READ canPaste NOTIFY canPasteChanged)

public:
    UCContentHub(QObject* parent = nullptr);
    ~UCContentHub() = default;

    Q_INVOKABLE void requestPaste(QQuickItem *targetItem);

    bool canPaste() const;
    QString getAppProfile() const;
    QMimeData* deserializeMimeData(const QByteArray &serializedMimeData);

Q_SIGNALS:
    void pasteSelected(QQuickItem *targetItem, const QString &data);
    void canPasteChanged();

private Q_SLOTS:
    void init();
    void onPasteSelected(const QString &appId, const QByteArray &mimedata, bool pasteAsRichText);
    void onPasteboardChanged();

private:
    void setCanPaste(bool value);
    QDBusInterface *m_dbusIface{nullptr};
    QDBusInterface *m_contentHubIface{nullptr};

    bool m_canPaste{false};
    QQuickItem *m_targetItem{nullptr};

    QDBusServiceWatcher * m_watcher{nullptr};
};

UT_NAMESPACE_END

#endif  // UCCONTENTHUB_P_H
