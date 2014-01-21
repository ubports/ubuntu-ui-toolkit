/*
 * Copyright 2014 Canonical Ltd.
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
 * Author: Florian Boucault <florian.boucault@canonical.com>
 */

#include "ucgraphmodel.h"
#include <QtCore/qmath.h>

UCGraphModel::UCGraphModel(QObject *parent) :
    QObject(parent),
    m_shift(0),
    m_samples(100)
{
    m_image = QImage(m_samples, 1, QImage::Format_RGB32);
    m_image.fill(0);
}

void UCGraphModel::appendValue(int width, int value)
{
    /* FIXME: modifying m_image here implicitly triggers a deep copy
       of its data because UCTextureFromImage usually holds a reference
       to that image
    */
    width = qMax(1, width);
    QRgb* line = (QRgb*)m_image.scanLine(0);

    if (width >= m_image.width()) {
        memset(&line[0], value, m_image.width() * 3);
    } else if (m_shift + width > m_image.width()) {
        int after = m_image.width() - m_shift;
        int before = width - after;
        memset(&line[m_shift], value, after * 3);
        memset(&line[0], value, before * 3);
    } else {
        memset(&line[m_shift], value, width * 3);
    }
    m_shift = (m_shift + width) % m_samples;

    Q_EMIT imageChanged();
    Q_EMIT shiftChanged();
}

QImage UCGraphModel::image() const
{
    return m_image;
}

int UCGraphModel::shift() const
{
    return m_shift;
}

int UCGraphModel::samples() const
{
    return m_samples;
}

void UCGraphModel::setSamples(int samples)
{
    if (samples != m_samples) {
        m_samples = samples;
        m_image = QImage(m_samples, 1, QImage::Format_RGB32);
        m_image.fill(0);
        Q_EMIT samplesChanged();
        Q_EMIT imageChanged();
    }
}
