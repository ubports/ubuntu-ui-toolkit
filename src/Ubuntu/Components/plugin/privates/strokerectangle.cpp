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
 * Author: Loïc Molinari <loic.molinari@canonical.com>
 */

#include "strokerectangle.h"

const QRgb defaultColor = qRgba(255, 255, 255, 255);
const float defaultSize = 50.0f;

// --- Shader ---

class StrokeShader : public QSGMaterialShader
{
public:
    StrokeShader();
    virtual char const* const* attributeNames() const;
    virtual void initialize();
    virtual void updateState(
        const RenderState& state, QSGMaterial* newEffect, QSGMaterial* oldEffect);

private:
    int m_matrixId;
    int m_opacityId;
};

StrokeShader::StrokeShader()
{
    setShaderSourceFile(
        QOpenGLShader::Vertex, QStringLiteral(":/uc/privates/shaders/strokerectangle.vert"));
    setShaderSourceFile(
        QOpenGLShader::Fragment, QStringLiteral(":/uc/privates/shaders/strokerectangle.frag"));
}

char const* const* StrokeShader::attributeNames() const
{
    static char const* const attributes[] = { "positionAttrib", "colorAttrib", 0 };
    return attributes;
}

void StrokeShader::initialize()
{
    QSGMaterialShader::initialize();
    m_matrixId = program()->uniformLocation("matrix");
    m_opacityId = program()->uniformLocation("opacity");
}

void StrokeShader::updateState(
    const RenderState& state, QSGMaterial* newEffect, QSGMaterial* oldEffect)
{
    Q_UNUSED(newEffect);
    Q_UNUSED(oldEffect);

    if (state.isMatrixDirty()) {
        program()->setUniformValue(m_matrixId, state.combinedMatrix());
    }
    if (state.isOpacityDirty()) {
        program()->setUniformValue(m_opacityId, state.opacity());
    }
}

// --- Material ---

UCStrokeRectangleMaterial::UCStrokeRectangleMaterial()
{
    setFlag(Blending, false);
}

QSGMaterialType* UCStrokeRectangleMaterial::type() const
{
    static QSGMaterialType type;
    return &type;
}

QSGMaterialShader* UCStrokeRectangleMaterial::createShader() const
{
    return new StrokeShader;
}

int UCStrokeRectangleMaterial::compare(const QSGMaterial* other) const
{
    Q_UNUSED(other);
    return 0;
}

// --- Node ---

UCStrokeRectangleNode::UCStrokeRectangleNode()
    : QSGGeometryNode()
    , m_material()
    , m_geometry(attributeSet(), 8, 10, GL_UNSIGNED_SHORT)
{
    memcpy(m_geometry.indexData(), indices(), 10 * sizeof(unsigned short));
    m_geometry.setDrawingMode(GL_TRIANGLE_STRIP);
    m_geometry.setIndexDataPattern(QSGGeometry::StaticPattern);
    m_geometry.setVertexDataPattern(QSGGeometry::AlwaysUploadPattern);
    setMaterial(&m_material);
    setGeometry(&m_geometry);
    qsgnode_set_description(this, QLatin1String("stroke"));
}

// static
const unsigned short* UCStrokeRectangleNode::indices()
{
    // The geometry is made of 8 vertices indexed with a triangle strip mode.
    //     0 ----- 1
    //     | 2 - 3 |
    //     | |   | |
    //     | 4 - 5 |
    //     6 ----- 7
    static const unsigned short indices[] = { 0, 2, 1, 3, 7, 5, 6, 4, 0, 2 };
    return indices;
}

// static
const QSGGeometry::AttributeSet& UCStrokeRectangleNode::attributeSet()
{
    static const QSGGeometry::Attribute attributes[] = {
        QSGGeometry::Attribute::create(0, 2, GL_FLOAT, true),
        QSGGeometry::Attribute::create(1, 4, GL_UNSIGNED_BYTE)
    };
    static const QSGGeometry::AttributeSet attributeSet = {
        2, sizeof(Vertex), attributes
    };
    return attributeSet;
}

// Pack a color in a premultiplied 32-bit ABGR value.
static quint32 packColor(QRgb color)
{
    const quint32 a = qAlpha(color);
    const quint32 b = ((qBlue(color) * a) + 0xff) >> 8;
    const quint32 g = ((qGreen(color) * a) + 0xff) >> 8;
    const quint32 r = ((qRed(color) * a) + 0xff) >> 8;
    return (a << 24) | ((b & 0xff) << 16) | ((g & 0xff) << 8) | (r & 0xff);
}

void UCStrokeRectangleNode::updateGeometry(const QSizeF& itemSize, float strokeSize, QRgb color)
{
    UCStrokeRectangleNode::Vertex* v =
        reinterpret_cast<UCStrokeRectangleNode::Vertex*>(m_geometry.vertexData());
    const float w = static_cast<float>(itemSize.width());
    const float h = static_cast<float>(itemSize.height());
    const float maxSize = qMin(w, h) * 0.5f;
    const float adaptedStrokeSize = qMin(strokeSize, maxSize);
    const quint32 packedColor = packColor(color);

    v[0].x = 0.0f;
    v[0].y = 0.0f;
    v[0].color = packedColor;
    v[1].x = w;
    v[1].y = 0.0f;
    v[1].color = packedColor;
    v[2].x = adaptedStrokeSize;
    v[2].y = adaptedStrokeSize;
    v[2].color = packedColor;
    v[3].x = w - adaptedStrokeSize;
    v[3].y = adaptedStrokeSize;
    v[3].color = packedColor;
    v[4].x = adaptedStrokeSize;
    v[4].y = h - adaptedStrokeSize;
    v[4].color = packedColor;
    v[5].x = w - adaptedStrokeSize;
    v[5].y = h - adaptedStrokeSize;
    v[5].color = packedColor;
    v[6].x = 0.0f;
    v[6].y = h;
    v[6].color = packedColor;
    v[7].x = w;
    v[7].y = h;
    v[7].color = packedColor;

    markDirty(QSGNode::DirtyGeometry);
}

// --- Item ---

UCStrokeRectangle::UCStrokeRectangle(QQuickItem* parent)
    : QQuickItem(parent)
    , m_color(defaultColor)
    , m_size(defaultSize)
{
    setFlag(ItemHasContents);
}

void UCStrokeRectangle::setSize(qreal size)
{
    size = qMax(0.0f, static_cast<float>(size));
    if (m_size != size) {
        m_size = size;
        update();
        Q_EMIT sizeChanged();
    }
}

void UCStrokeRectangle::setColor(const QColor& color)
{
    const QRgb rgbColor = qRgba(color.red(), color.green(), color.blue(), color.alpha());
    if (m_color != rgbColor) {
        m_color = rgbColor;
        update();
        Q_EMIT colorChanged();
    }
}

QSGNode* UCStrokeRectangle::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data)
{
    Q_UNUSED(data);

    const QSizeF itemSize(width(), height());
    if (itemSize.isEmpty() || m_size <= 0.0f) {
        delete oldNode;
        return NULL;
    }

    UCStrokeRectangleNode* node =
        oldNode ? static_cast<UCStrokeRectangleNode*>(oldNode) : new UCStrokeRectangleNode;
    node->updateGeometry(itemSize, m_size, m_color);

    return node;
}
