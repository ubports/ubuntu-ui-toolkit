/*
 * Copyright 2013 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Jussi Pakkanen <jussi.pakkanen@canonical.com>
*/

#include "thumbnailgenerator.h"
#include <stdexcept>
#include <QDebug>
#include <QMimeDatabase>
#include <QUrl>

const char *DEFAULT_VIDEO_ART = "/usr/share/unity/icons/video_missing.png";
const char *DEFAULT_ALBUM_ART = "/usr/share/unity/icons/album_missing.png";

ThumbnailGenerator::ThumbnailGenerator() : QQuickImageProvider(QQuickImageProvider::Image,
        QQmlImageProviderBase::ForceAsynchronousImageLoading) {

}

QImage ThumbnailGenerator::requestImage(const QString &id, QSize *realSize,
        const QSize &requestedSize) {
    /* Allow appending a query string (e.g. ?something=timestamp)
     * to the id and then ignore it.
     * This is workaround to force reloading a thumbnail when it has
     * the same file name on disk but we know the content has changed.
     * It is necessary because in such a situation the QML image cache
     * will kick in and this ImageProvider will never get called.
     * The only "solution" is setting Image.cache = false, but in some
     * cases we don't want to do that for performance reasons, so this
     * is the only way around the issue for now. */
    std::string src_path(QUrl(id).path().toUtf8().data());
    std::string tgt_path;
    try {
        ThumbnailSize desiredSize;
        const int large_cutoff = 256;
        const int small_cutoff = 128;
        if(requestedSize.width() > large_cutoff || requestedSize.height() > large_cutoff) {
            desiredSize = TN_SIZE_ORIGINAL;
        } else if(requestedSize.width() > small_cutoff || requestedSize.height() > small_cutoff) {
            desiredSize = TN_SIZE_LARGE;
        } else {
            desiredSize = TN_SIZE_SMALL;
        }
        tgt_path = tn.get_thumbnail(src_path, desiredSize);
        if(!tgt_path.empty()) {
            QString tgt(tgt_path.c_str());
            QImage image;
            image.load(tgt);
            *realSize = image.size();
            return image;
        }
    } catch(std::runtime_error &e) {
        qDebug() << "Thumbnail generator failed: " << e.what();
    }
    return getFallbackImage(id, realSize, requestedSize);
}

QImage ThumbnailGenerator::getFallbackImage(const QString &id, QSize *size,
        const QSize &requestedSize) {
    Q_UNUSED(requestedSize);
    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForFile(id);
    QImage result;
    if(mime.name().contains("audio")) {
        result.load(DEFAULT_ALBUM_ART);
    } else if(mime.name().contains("video")) {
        result.load(DEFAULT_VIDEO_ART);
    }
    *size = result.size();
    return result;
}
