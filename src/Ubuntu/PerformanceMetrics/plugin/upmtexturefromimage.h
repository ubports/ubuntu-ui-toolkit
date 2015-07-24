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

#ifndef UPMTEXTUREFROMIMAGE_H
#define UPMTEXTUREFROMIMAGE_H

#include <QtQuick/QSGTextureProvider>
#include <QtQuick/QQuickItem>

class UPMTextureFromImageTextureProvider : public QSGTextureProvider
{
    Q_OBJECT

public:
    explicit UPMTextureFromImageTextureProvider();
    virtual ~UPMTextureFromImageTextureProvider();
    QSGTexture* texture() const;
    void setTexture(QSGTexture* texture);

private:
    QSGTexture* m_texture;
};


class UPMTextureFromImage : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QImage image READ image WRITE setImage NOTIFY imageChanged)

public:
    explicit UPMTextureFromImage(QQuickItem* parent = 0);
    virtual ~UPMTextureFromImage();
    bool isTextureProvider() const;
    QSGTextureProvider* textureProvider() const;
    QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* updatePaintNodeData);

    // getter
    QImage image() const;

    // setters
    void setImage(QImage image);

Q_SIGNALS:
    void imageChanged();

private:
    UPMTextureFromImageTextureProvider* m_textureProvider;
    QImage m_image;
    bool m_textureNeedsUpdate;
};


#endif // UPMTEXTUREFROMIMAGE_H
