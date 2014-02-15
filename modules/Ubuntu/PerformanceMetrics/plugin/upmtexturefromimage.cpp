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

#include <QtQuick/QQuickWindow>
#include "upmtexturefromimage.h"

UPMTextureFromImageTextureProvider::UPMTextureFromImageTextureProvider() :
    QSGTextureProvider(),
    m_texture(NULL)
{
}

UPMTextureFromImageTextureProvider::~UPMTextureFromImageTextureProvider()
{
    if (m_texture != NULL) {
        delete m_texture;
    }
}

QSGTexture* UPMTextureFromImageTextureProvider::texture() const
{
    return m_texture;
}

void UPMTextureFromImageTextureProvider::setTexture(QSGTexture* texture)
{
    if (m_texture != NULL) {
        delete m_texture;
    }
    m_texture = texture;

    // FIXME: hardcoded flag
    if (m_texture != NULL) {
        m_texture->setHorizontalWrapMode(QSGTexture::Repeat);
    }
}



UPMTextureFromImage::UPMTextureFromImage(QQuickItem* parent) :
    QQuickItem(parent),
    m_textureProvider(NULL),
    m_textureNeedsUpdate(true)
{
    setFlag(QQuickItem::ItemHasContents);
}

UPMTextureFromImage::~UPMTextureFromImage()
{
    if (m_textureProvider != NULL) {
        m_textureProvider->deleteLater();
    }
}

bool UPMTextureFromImage::isTextureProvider() const
{
    return true;
}

QSGTextureProvider* UPMTextureFromImage::textureProvider() const
{
    if (m_textureProvider == NULL) {
        const_cast<UPMTextureFromImage*>(this)->m_textureProvider = new UPMTextureFromImageTextureProvider;
        m_textureProvider->setTexture(window()->createTextureFromImage(m_image));
    }
    return m_textureProvider;
}

QSGNode* UPMTextureFromImage::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* updatePaintNodeData)
{
    Q_UNUSED(oldNode)
    Q_UNUSED(updatePaintNodeData)

    if (m_textureNeedsUpdate && m_textureProvider != NULL) {
        m_textureProvider->setTexture(window()->createTextureFromImage(m_image));
        m_textureNeedsUpdate = false;
    }
    return NULL;
}

QImage UPMTextureFromImage::image() const
{
    return m_image;
}

void UPMTextureFromImage::setImage(QImage image)
{
    if (image != m_image) {
        m_image = image;
        Q_EMIT imageChanged();
        m_textureNeedsUpdate = true;
        update();
    }
}
