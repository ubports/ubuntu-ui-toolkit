#include "giconprovider.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtGui/QIcon>

extern "C" {
#include <gio/gio.h>
}

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
    g_type_init();
}

QImage GIconProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    QPixmap result;
    QByteArray utf8Name = id.toUtf8();
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
                result = themedIcon.pixmap(requestedSize);
            } else {
                result = themedIcon.pixmap(themedIcon.availableSizes().last());
            }
        } else {
            qDebug() << "Fail to load themed icon for:" << id;
        }
    } else if (G_IS_FILE_ICON(icon)) {
        gchar *iconName = g_icon_to_string(icon);
        if (QFile::exists(iconName)) {
            result = QPixmap(iconName);
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
    return result.toImage();
}
