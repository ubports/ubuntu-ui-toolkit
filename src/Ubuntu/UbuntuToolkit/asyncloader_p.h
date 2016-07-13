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
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#ifndef ASYNCLOADER_H
#define ASYNCLOADER_H

#include <QtQml/QQmlComponent>
#include <ubuntutoolkitglobal.h>

class QQuickItem;
class QQmlContext;

UT_NAMESPACE_BEGIN

class AsyncLoaderPrivate;
class UBUNTUTOOLKIT_EXPORT AsyncLoader : public QObject
{
    Q_OBJECT
public:
    enum LoadingStatus {
        Null,
        Compiling,
        Loading,
        Initializing,
        Ready,
        Error,
        Reset
    };

    explicit AsyncLoader(QObject *parent = 0);
    ~AsyncLoader();

    bool load(const QUrl &url, QQmlContext *context);
    bool load(QQmlComponent *component, QQmlContext *context);
    bool reset();
    LoadingStatus status();
    void forceCompletion();

Q_SIGNALS:
    void loadingStatus(AsyncLoader::LoadingStatus status, QObject *object);

protected:
    Q_DECLARE_PRIVATE(AsyncLoader)
};

UT_NAMESPACE_END

#endif // ASYNCLOADER_H
