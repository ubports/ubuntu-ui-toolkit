/*
 * Copyright 2012 Canonical Ltd.
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
 * Author: Renato Araujo Oliveira Filho <renato.filho@canonical.com>
 */

#include "giconprovider.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtGui/QIcon>

extern "C" {
#include <gio/gio.h>
}

/*
  GIconProvider is a implementation of QQuickImageProvider that handles GIcon sources.
  GIcons sources can be loaded in QML using the uri: "image://gicon/<icon_name>"

  Example:
  \qml
    Image {
      source: "image://gicon/my_icon_name"
    }
  \endqml
*/

static QIcon getThemedIcon(GThemedIcon *icon)
{
    const gchar* const* iconNames = g_themed_icon_get_names(icon);
    guint index = 0;
    while(iconNames[index] != NULL) {
        if (QIcon::hasThemeIcon(iconNames[index])) {
            return QIcon::fromTheme(iconNames[index]);
        }
        index++;
    }
    return QIcon();
}

GIconProvider::GIconProvider()
    : QQuickImageProvider(QQuickImageProvider::Image)
{
#if !GLIB_CHECK_VERSION(2, 36, 0)
    g_type_init();  // Deprecated starting from GLib 2.36.
#endif
}

QImage GIconProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    static bool warned = false;

    if (!warned) {
        qWarning("WARNING: The 'gicon' image provider is deprecated and will be removed soon. "
                 "Please use file:// urls for file icons, and the 'theme' image provider for "
                 "themed icons (image://theme/iconname,fallback1,fallback2).");
        warned = true;
    }

    QImage result;
    QByteArray utf8Name = QUrl::fromPercentEncoding(id.toUtf8()).toUtf8();
    GError *error = NULL;
    GIcon *icon = g_icon_new_for_string(utf8Name.data(), &error);
    if (error) {
        qWarning() << "Fail to load icon: " << id << error->message;
        g_error_free(error);
        return QImage();
    }

    if (G_IS_THEMED_ICON(icon)) {
        QIcon themedIcon = getThemedIcon(reinterpret_cast<GThemedIcon*>(icon));
        if (!themedIcon.isNull()) {
            if (requestedSize.isValid()) {
                result = themedIcon.pixmap(requestedSize).toImage();
            } else {
                result = themedIcon.pixmap(themedIcon.availableSizes().last()).toImage();
            }
        } else {
            qDebug() << "Fail to load themed icon for:" << id;
        }
    } else if (G_IS_FILE_ICON(icon)) {
        gchar *iconName = g_icon_to_string(icon);
        if (QFile::exists(iconName)) {
            result.load(iconName);
            if (requestedSize.isValid()) {
                result = result.scaled(requestedSize);
            }
        } else {
            qWarning() << "File does not exists:" << iconName;
        }

        g_free(iconName);
    } else {
        qWarning() << "Invalid icon format:" << id;
    }

    g_object_unref(icon);
    *size = result.size();
    return result;
}
