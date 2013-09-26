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

ThumbnailGenerator::ThumbnailGenerator() : QQuickImageProvider(QQuickImageProvider::Pixmap) {

}

QPixmap ThumbnailGenerator::requestPixmap(const QString &id, QSize *realSize,
        const QSize &/*requestedSize*/) {
    std::string src_path(id.toUtf8().data());
    std::string tgt_path;
    try {
        tgt_path = tn.get_thumbnail(src_path, TN_SIZE_SMALL);
        if(!tgt_path.empty()) {
            QString tgt(tgt_path.c_str());
            QPixmap image;
            image.load(tgt);
            *realSize = image.size();
            return image;
        }
    } catch(std::runtime_error &e) {
        // thumbnail generation failed for some reason
        // so just return default image
    }
    *realSize = QSize(0, 0);
    return QPixmap();
}
