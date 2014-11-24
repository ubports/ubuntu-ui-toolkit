/*
 * Copyright 2013-2014 Canonical Ltd.
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

#include "ucubuntushape.h"
#include "ucubuntushapetexture.h"
#include "ucunits.h"
#include <QtCore/QPointer>
#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGTextureProvider>
#include <QtQuick/qsgflatcolormaterial.h>
#include <QtQuick/private/qquickimage_p.h>
#include <math.h>

class ShapeMaterial : public QSGMaterial
{
public:
    struct Data
    {
        // Flags must be kept in sync with GLSL fragment shader.
        enum {
            Textured             = (1 << 0),
            Overlaid             = (1 << 1),
            HorizontallyRepeated = (1 << 2),
            VerticallyRepeated   = (1 << 3),
            Repeated             = (HorizontallyRepeated | VerticallyRepeated)
        };
        QSGTexture* shapeTexture;
        QSGTextureProvider* sourceTextureProvider;
        quint8 sourceOpacity;
        QRgb overlayColor;
        quint16 overlaySteps[4];
        QSGTexture::Filtering shapeTextureFiltering;
        quint8 flags;
    };

    ShapeMaterial();
    virtual QSGMaterialType* type() const;
    virtual QSGMaterialShader* createShader() const;
    virtual int compare(const QSGMaterial* other) const;
    const Data* constData() const { return &data_; }
    Data* data() { return &data_; }

private:
    // UCUbuntuShape::updatePaintNode() directly writes to data and ShapeShader::updateState()
    // directly reads from it. We don't bother with getters/setters since it's only meant to be used
    // by the UbuntuShape implementation and makes maintainance easier.
    Data data_;
};

// -- Scene graph shader ---

class ShapeShader : public QSGMaterialShader
{
public:
    ShapeShader();
    virtual char const* const* attributeNames() const;
    virtual void initialize();
    virtual void updateState(
        const RenderState& state, QSGMaterial* newEffect, QSGMaterial* oldEffect);

private:
    QOpenGLFunctions* functions_;
    int matrixId_;
    int opacityId_;
    int sourceOpacityId_;
    int secondaryBackgroundColorId_;
    int overlayColorId_;
    int overlayStepsId_;
    int flagsId_;
};

ShapeShader::ShapeShader()
    : QSGMaterialShader()
{
    setShaderSourceFile(QOpenGLShader::Vertex, QStringLiteral(":/shaders/ucubuntushape.vert"));
    setShaderSourceFile(QOpenGLShader::Fragment, QStringLiteral(":/shaders/ucubuntushape.frag"));
}

char const* const* ShapeShader::attributeNames() const
{
    static char const* const attributes[] = {
        "positionAttrib", "shapeCoordAttrib", "sourceCoordAttrib", "backgroundColorAttrib", 0
    };
    return attributes;
}

void ShapeShader::initialize()
{
    QSGMaterialShader::initialize();

    program()->bind();
    program()->setUniformValue("shapeTexture", 0);
    program()->setUniformValue("sourceTexture", 1);

    functions_ = QOpenGLContext::currentContext()->functions();
    matrixId_ = program()->uniformLocation("matrix");
    opacityId_ = program()->uniformLocation("opacity");
    sourceOpacityId_ = program()->uniformLocation("sourceOpacity");
    overlayColorId_ = program()->uniformLocation("overlayColor");
    overlayStepsId_ = program()->uniformLocation("overlaySteps");
    flagsId_ = program()->uniformLocation("flags");
}

void ShapeShader::updateState(const RenderState& state, QSGMaterial* newEffect,
                              QSGMaterial* oldEffect)
{
    Q_UNUSED(oldEffect);

    const float u8toF32 = 1.0f / 255.0f;
    const float u16ToF32 = 1.0f / static_cast<float>(0xffff);
    const ShapeMaterial::Data* data = static_cast<ShapeMaterial*>(newEffect)->constData();

    // Bind shape texture.
    QSGTexture* shapeTexture = data->shapeTexture;
    if (shapeTexture) {
        shapeTexture->setFiltering(data->shapeTextureFiltering);
        shapeTexture->setHorizontalWrapMode(QSGTexture::ClampToEdge);
        shapeTexture->setVerticalWrapMode(QSGTexture::ClampToEdge);
        shapeTexture->bind();
    } else {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (data->flags & ShapeMaterial::Data::Textured) {
        // Bind image texture.
        functions_->glActiveTexture(GL_TEXTURE1);
        QSGTextureProvider* provider = data->sourceTextureProvider;
        QSGTexture* texture = provider ? provider->texture() : NULL;
        if (texture) {
            if (data->flags & ShapeMaterial::Data::Repeated) {
                if (texture->isAtlasTexture()) {
                    // A texture in an atlas can't be repeated with builtin GPU facility (exposed by
                    // GL_REPEAT with OpenGL), so we extract it and create a new dedicated one.
                    texture = texture->removedFromAtlas();
                }
                texture->setHorizontalWrapMode(
                    data->flags & ShapeMaterial::Data::HorizontallyRepeated ?
                    QSGTexture::Repeat : QSGTexture::ClampToEdge);
                texture->setVerticalWrapMode(
                    data->flags & ShapeMaterial::Data::VerticallyRepeated ?
                    QSGTexture::Repeat : QSGTexture::ClampToEdge);
            }
            texture->bind();
        } else {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        functions_->glActiveTexture(GL_TEXTURE0);
        // Update image uniform.
        program()->setUniformValue(sourceOpacityId_, data->sourceOpacity * u8toF32);
    }

    if (data->flags & ShapeMaterial::Data::Overlaid) {
        // Update overlay uniforms.
        const QRgb c = data->overlayColor;
        program()->setUniformValue(overlayColorId_, QVector4D(
            qRed(c) * u8toF32, qGreen(c) * u8toF32, qBlue(c) * u8toF32, qAlpha(c) * u8toF32));
        program()->setUniformValue(overlayStepsId_, QVector4D(
            data->overlaySteps[0] * u16ToF32, data->overlaySteps[1] * u16ToF32,
            data->overlaySteps[2] * u16ToF32, data->overlaySteps[3] * u16ToF32));
    }

    program()->setUniformValue(flagsId_, data->flags);

    // Update QtQuick engine uniforms.
    if (state.isMatrixDirty()) {
        program()->setUniformValue(matrixId_, state.combinedMatrix());
    }
    if (state.isOpacityDirty()) {
        program()->setUniformValue(opacityId_, state.opacity());
    }
}

// --- Scene graph material ---

ShapeMaterial::ShapeMaterial()
    : data_()
{
    setFlag(Blending);
}

QSGMaterialType* ShapeMaterial::type() const
{
    static QSGMaterialType type;
    return &type;
}

QSGMaterialShader* ShapeMaterial::createShader() const
{
    return new ShapeShader;
}

int ShapeMaterial::compare(const QSGMaterial* other) const
{
    const ShapeMaterial::Data* otherData = static_cast<const ShapeMaterial*>(other)->constData();
    const int diff = memcmp(&data_, otherData, sizeof(ShapeMaterial::Data));
    // Repeat wrap modes require textures to be extracted from their atlases. Since we just store
    // the texture provider in the material data (not the texture as we want to do the extracion at
    // QSGShader::updateState() time), we make the comparison fail when repeat wrapping is set.
    return diff | (data_.flags & ShapeMaterial::Data::Repeated);
}

// --- Scene graph node ---

class ShapeNode : public QSGGeometryNode
{
public:
    // FIXME(loicm) Storing lower precision data types could be more efficent. On PowerVR, for
    //     instance, that requires a conversion so the trade-off between shader cycles and bandwidth
    //     requirements must be verified.
    struct Vertex {
        float position[2];
        float shapeCoordinate[2];
        float sourceCoordinate[4];
        quint32 backgroundColor;
    };

    ShapeNode(UCUbuntuShape* item);
    ShapeMaterial* material() { return &material_; }
    void setVertices(float width, float height, float radius, float shapeCoordinate[][2],
                     UCUbuntuShape::WrapMode hWrap, UCUbuntuShape::WrapMode vWrap,
                     const QVector4D& sourceTransform, const QRectF& textureRect,
                     QRgb backgroundColor, QRgb secondaryBackgroundColor);

private:
    UCUbuntuShape* item_;
    QSGGeometry geometry_;
    ShapeMaterial material_;
};

static const unsigned short shapeMeshIndices[] __attribute__((aligned(16))) = {
    0, 4, 1, 5, 2, 6, 3, 7,       // Triangles 1 to 6.
    7, 4,                         // Degenerate triangles.
    4, 8, 5, 9, 6, 10, 7, 11,     // Triangles 7 to 12.
    11, 8,                        // Degenerate triangles.
    8, 12, 9, 13, 10, 14, 11, 15  // Triangles 13 to 18
};

#define ARRAY_SIZE(a) \
    ((sizeof(a) / sizeof(*(a))) / static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))

static const struct {
    const unsigned short* const indices;
    int indexCount;           // Number of indices.
    int vertexCount;          // Number of vertices.
    int attributeCount;       // Number of attributes.
    int stride;               // Offset in bytes from one vertex to the other.
    int positionCount;        // Number of components per position.
    int positionType;         // OpenGL type of the position components.
    int shapeCoordCount;      // Number of components per shape texture coordinate.
    int shapeCoordType;       // OpenGL type of the shape texture coordinate components.
    int sourceCoordCount;     // Number of components per source texture coordinate.
    int sourceCoordType;      // OpenGL type of the source texture coordinate components.
    int backgroundColorCount; // Number of components per quad coordinate.
    int backgroundColorType;  // OpenGL type of the quad coordinate components.
    int indexType;            // OpenGL type of the indices.
} shapeMesh = {
    shapeMeshIndices, ARRAY_SIZE(shapeMeshIndices),
    16, 4, sizeof(ShapeNode::Vertex), 2, GL_FLOAT, 2, GL_FLOAT, 4, GL_FLOAT, 4, GL_UNSIGNED_BYTE,
    GL_UNSIGNED_SHORT
};

static const QSGGeometry::AttributeSet& getAttributes()
{
    static QSGGeometry::Attribute data[] = {
        QSGGeometry::Attribute::create(0, shapeMesh.positionCount, shapeMesh.positionType, true),
        QSGGeometry::Attribute::create(1, shapeMesh.shapeCoordCount, shapeMesh.shapeCoordType),
        QSGGeometry::Attribute::create(2, shapeMesh.sourceCoordCount, shapeMesh.sourceCoordType),
        QSGGeometry::Attribute::create(3, shapeMesh.backgroundColorCount,
                                       shapeMesh.backgroundColorType)
    };
    static QSGGeometry::AttributeSet attributes = {
        shapeMesh.attributeCount, shapeMesh.stride, data
    };
    return attributes;
}

// Gets the size in bytes of an OpenGL type in the range [GL_BYTE, GL_DOUBLE].
static int sizeOfType(GLenum type)
{
    static int sizes[] = {
        sizeof(char), sizeof(unsigned char), sizeof(short), sizeof(unsigned short), sizeof(int),
        sizeof(unsigned int), sizeof(float), 2, 3, 4, sizeof(double)
    };
    Q_ASSERT(type >= 0x1400 && type <= 0x140a);
    return sizes[type - 0x1400];
}

ShapeNode::ShapeNode(UCUbuntuShape* item)
    : QSGGeometryNode()
    , item_(item)
    , geometry_(getAttributes(), shapeMesh.vertexCount, shapeMesh.indexCount, shapeMesh.indexType)
    , material_()
{
    memcpy(geometry_.indexData(), shapeMesh.indices,
           shapeMesh.indexCount * sizeOfType(shapeMesh.indexType));
    geometry_.setDrawingMode(GL_TRIANGLE_STRIP);
    geometry_.setIndexDataPattern(QSGGeometry::StaticPattern);
    geometry_.setVertexDataPattern(QSGGeometry::AlwaysUploadPattern);
    setGeometry(&geometry_);
    setMaterial(&material_);
    setFlag(UsePreprocess, false);
}

// Pack a premultiplied 32-bit ABGR integer.
static quint32 packPremultipliedABGR32(quint32 a, quint32 b, quint32 g, quint32 r)
{
    const quint32 pb = ((b * a) + 0xff) >> 8;
    const quint32 pg = ((g * a) + 0xff) >> 8;
    const quint32 pr = ((r * a) + 0xff) >> 8;
    return (a << 24) | ((pb & 0xff) << 16) | ((pg & 0xff) << 8) | (pr & 0xff);
}

// Lerp c1 and c2 with t in the range [0, 255]. Return value is a premultiplied 32-bit ABGR integer.
static quint32 lerpColor(qint32 t, QRgb c1, QRgb c2)
{
    const quint32 a = qAlpha(c1) + ((t * (qAlpha(c2) - qAlpha(c1)) + 0xff) >> 8);
    const quint32 b = qBlue(c1) + ((t * (qBlue(c2) - qBlue(c1)) + 0xff) >> 8);
    const quint32 g = qGreen(c1) + ((t * (qGreen(c2) - qGreen(c1)) + 0xff) >> 8);
    const quint32 r = qRed(c1) + ((t * (qRed(c2) - qRed(c1)) + 0xff) >> 8);
    return packPremultipliedABGR32(a, b, g, r);
}

void ShapeNode::setVertices(float width, float height, float radius, float shapeCoordinate[][2],
                            UCUbuntuShape::WrapMode hWrap, UCUbuntuShape::WrapMode vWrap,
                            const QVector4D& sourceTransform, const QRectF& textureRect,
                            QRgb backgroundColor, QRgb secondaryBackgroundColor)
{
    Vertex* vertices = reinterpret_cast<ShapeNode::Vertex*>(geometry_.vertexData());
    const float radiusW = radius / width;
    const float radiusH = radius / height;

    // Get transformation for source texture coordinates.
    const float sourceSx1 = sourceTransform.x() * textureRect.width();
    const float sourceSy1 = sourceTransform.y() * textureRect.height();
    const float sourceTx1 = sourceTransform.z() * textureRect.width() + textureRect.x();
    const float sourceTy1 = sourceTransform.w() * textureRect.height() + textureRect.y();

    // Get transformation for source mask coordinates. In case of a repeat wrap mode, the
    // transformation is made so that the mask takes the whole area.
    float sourceSx2, sourceTx2, sourceSy2, sourceTy2;
    if (hWrap == UCUbuntuShape::Transparent) {
        sourceSx2 = sourceTransform.x() * 2.0f;
        sourceTx2 = sourceTransform.z() * 2.0f - 1.0f;
    } else {
        sourceSx2 = 2.0f;
        sourceTx2 = -1.0f;
    }
    if (vWrap == UCUbuntuShape::Transparent) {
        sourceSy2 = sourceTransform.y() * 2.0f;
        sourceTy2 = sourceTransform.w() * 2.0f - 1.0f;
    } else {
        sourceSy2 = 2.0f;
        sourceTy2 = -1.0f;
    }

    // Get colors in 32-bit premultiplied ABGR format.
    const float f32toS32 = 255.0f;
    const quint32 backgroundColorRow1 = packPremultipliedABGR32(
        qAlpha(backgroundColor), qBlue(backgroundColor), qGreen(backgroundColor),
        qRed(backgroundColor));
    const quint32 backgroundColorRow2 = lerpColor(
        radiusH * f32toS32, backgroundColor, secondaryBackgroundColor);
    const quint32 backgroundColorRow3 = lerpColor(
        (1.0f - radiusH) * f32toS32, backgroundColor, secondaryBackgroundColor);
    const quint32 backgroundColorRow4 = packPremultipliedABGR32(
        qAlpha(secondaryBackgroundColor), qBlue(secondaryBackgroundColor),
        qGreen(secondaryBackgroundColor), qRed(secondaryBackgroundColor));

    // Set top row of 4 vertices.
    vertices[0].position[0] = 0.0f;
    vertices[0].position[1] = 0.0f;
    vertices[0].shapeCoordinate[0] = shapeCoordinate[0][0];
    vertices[0].shapeCoordinate[1] = shapeCoordinate[0][1];
    vertices[0].sourceCoordinate[0] = sourceTx1;
    vertices[0].sourceCoordinate[1] = sourceTy1;
    vertices[0].sourceCoordinate[2] = sourceTx2;
    vertices[0].sourceCoordinate[3] = sourceTy2;
    vertices[0].backgroundColor = backgroundColorRow1;
    vertices[1].position[0] = radius;
    vertices[1].position[1] = 0.0f;
    vertices[1].shapeCoordinate[0] = shapeCoordinate[1][0];
    vertices[1].shapeCoordinate[1] = shapeCoordinate[1][1];
    vertices[1].sourceCoordinate[0] = radiusW * sourceSx1 + sourceTx1;
    vertices[1].sourceCoordinate[1] = sourceTy1;
    vertices[1].sourceCoordinate[2] = radiusW * sourceSx2 + sourceTx2;
    vertices[1].sourceCoordinate[3] = sourceTy2;
    vertices[1].backgroundColor = backgroundColorRow1;
    vertices[2].position[0] = width - radius;
    vertices[2].position[1] = 0.0f;
    vertices[2].shapeCoordinate[0] = shapeCoordinate[2][0];
    vertices[2].shapeCoordinate[1] = shapeCoordinate[2][1];
    vertices[2].sourceCoordinate[0] = (1.0f - radiusW) * sourceSx1 + sourceTx1;
    vertices[2].sourceCoordinate[1] = sourceTy1;
    vertices[2].sourceCoordinate[2] = (1.0f - radiusW) * sourceSx2 + sourceTx2;
    vertices[2].sourceCoordinate[3] = sourceTy2;
    vertices[2].backgroundColor = backgroundColorRow1;
    vertices[3].position[0] = width;
    vertices[3].position[1] = 0.0f;
    vertices[3].shapeCoordinate[0] = shapeCoordinate[3][0];
    vertices[3].shapeCoordinate[1] = shapeCoordinate[3][1];
    vertices[3].sourceCoordinate[0] = sourceSx1 + sourceTx1;
    vertices[3].sourceCoordinate[1] = sourceTy1;
    vertices[3].sourceCoordinate[2] = sourceSx2 + sourceTx2;
    vertices[3].sourceCoordinate[3] = sourceTy2;
    vertices[3].backgroundColor = backgroundColorRow1;

    // Set middle-top row of 4 vertices.
    vertices[4].position[0] = 0.0f;
    vertices[4].position[1] = radius;
    vertices[4].shapeCoordinate[0] = shapeCoordinate[4][0];
    vertices[4].shapeCoordinate[1] = shapeCoordinate[4][1];
    vertices[4].sourceCoordinate[0] = sourceTx1;
    vertices[4].sourceCoordinate[1] = radiusH * sourceSy1 + sourceTy1;
    vertices[4].sourceCoordinate[2] = sourceTx2;
    vertices[4].sourceCoordinate[3] = radiusH * sourceSy2 + sourceTy2;
    vertices[4].backgroundColor = backgroundColorRow2;
    vertices[5].position[0] = radius;
    vertices[5].position[1] = radius;
    vertices[5].shapeCoordinate[0] = shapeCoordinate[5][0];
    vertices[5].shapeCoordinate[1] = shapeCoordinate[5][1];
    vertices[5].sourceCoordinate[0] = radiusW * sourceSx1 + sourceTx1;
    vertices[5].sourceCoordinate[1] = radiusH * sourceSy1 + sourceTy1;
    vertices[5].sourceCoordinate[2] = radiusW * sourceSx2 + sourceTx2;
    vertices[5].sourceCoordinate[3] = radiusH * sourceSy2 + sourceTy2;
    vertices[5].backgroundColor = backgroundColorRow2;
    vertices[6].position[0] = width - radius;
    vertices[6].position[1] = radius;
    vertices[6].shapeCoordinate[0] = shapeCoordinate[6][0];
    vertices[6].shapeCoordinate[1] = shapeCoordinate[6][1];
    vertices[6].sourceCoordinate[0] = (1.0f - radiusW) * sourceSx1 + sourceTx1;
    vertices[6].sourceCoordinate[1] = radiusH * sourceSy1 + sourceTy1;
    vertices[6].sourceCoordinate[2] = (1.0f - radiusW) * sourceSx2 + sourceTx2;
    vertices[6].sourceCoordinate[3] = radiusH * sourceSy2 + sourceTy2;
    vertices[6].backgroundColor = backgroundColorRow2;
    vertices[7].position[0] = width;
    vertices[7].position[1] = radius;
    vertices[7].shapeCoordinate[0] = shapeCoordinate[7][0];
    vertices[7].shapeCoordinate[1] = shapeCoordinate[7][1];
    vertices[7].sourceCoordinate[0] = sourceSx1 + sourceTx1;
    vertices[7].sourceCoordinate[1] = radiusH * sourceSy1 + sourceTy1;
    vertices[7].sourceCoordinate[2] = sourceSx2 + sourceTx2;
    vertices[7].sourceCoordinate[3] = radiusH * sourceSy2 + sourceTy2;
    vertices[7].backgroundColor = backgroundColorRow2;

    // Set middle-bottom row of 4 vertices.
    vertices[8].position[0] = 0.0f;
    vertices[8].position[1] = height - radius;
    vertices[8].shapeCoordinate[0] = shapeCoordinate[8][0];
    vertices[8].shapeCoordinate[1] = shapeCoordinate[8][1];
    vertices[8].sourceCoordinate[0] = sourceTx1;
    vertices[8].sourceCoordinate[1] = (1.0f - radiusH) * sourceSy1 + sourceTy1;
    vertices[8].sourceCoordinate[2] = sourceTx2;
    vertices[8].sourceCoordinate[3] = (1.0f - radiusH) * sourceSy2 + sourceTy2;
    vertices[8].backgroundColor = backgroundColorRow3;
    vertices[9].position[0] = radius;
    vertices[9].position[1] = height - radius;
    vertices[9].shapeCoordinate[0] = shapeCoordinate[9][0];
    vertices[9].shapeCoordinate[1] = shapeCoordinate[9][1];
    vertices[9].sourceCoordinate[0] = radiusW * sourceSx1 + sourceTx1;
    vertices[9].sourceCoordinate[1] = (1.0f - radiusH) * sourceSy1 + sourceTy1;
    vertices[9].sourceCoordinate[2] = radiusW * sourceSx2 + sourceTx2;
    vertices[9].sourceCoordinate[3] = (1.0f - radiusH) * sourceSy2 + sourceTy2;
    vertices[9].backgroundColor = backgroundColorRow3;
    vertices[10].position[0] = width - radius;
    vertices[10].position[1] = height - radius;
    vertices[10].shapeCoordinate[0] = shapeCoordinate[10][0];
    vertices[10].shapeCoordinate[1] = shapeCoordinate[10][1];
    vertices[10].sourceCoordinate[0] = (1.0f - radiusW) * sourceSx1 + sourceTx1;
    vertices[10].sourceCoordinate[1] = (1.0f - radiusH) * sourceSy1 + sourceTy1;
    vertices[10].sourceCoordinate[2] = (1.0f - radiusW) * sourceSx2 + sourceTx2;
    vertices[10].sourceCoordinate[3] = (1.0f - radiusH) * sourceSy2 + sourceTy2;
    vertices[10].backgroundColor = backgroundColorRow3;
    vertices[11].position[0] = width;
    vertices[11].position[1] = height - radius;
    vertices[11].shapeCoordinate[0] = shapeCoordinate[11][0];
    vertices[11].shapeCoordinate[1] = shapeCoordinate[11][1];
    vertices[11].sourceCoordinate[0] = sourceSx1 + sourceTx1;
    vertices[11].sourceCoordinate[1] = (1.0f - radiusH) * sourceSy1 + sourceTy1;
    vertices[11].sourceCoordinate[2] = sourceSx2 + sourceTx2;
    vertices[11].sourceCoordinate[3] = (1.0f - radiusH) * sourceSy2 + sourceTy2;
    vertices[11].backgroundColor = backgroundColorRow3;

    // Set bottom row of 4 vertices.
    vertices[12].position[0] = 0.0f;
    vertices[12].position[1] = height;
    vertices[12].shapeCoordinate[0] = shapeCoordinate[12][0];
    vertices[12].shapeCoordinate[1] = shapeCoordinate[12][1];
    vertices[12].sourceCoordinate[0] = sourceTx1;
    vertices[12].sourceCoordinate[1] = sourceSy1 + sourceTy1;
    vertices[12].sourceCoordinate[2] = sourceTx2;
    vertices[12].sourceCoordinate[3] = sourceSy2 + sourceTy2;
    vertices[12].backgroundColor = backgroundColorRow4;
    vertices[13].position[0] = radius;
    vertices[13].position[1] = height;
    vertices[13].shapeCoordinate[0] = shapeCoordinate[13][0];
    vertices[13].shapeCoordinate[1] = shapeCoordinate[13][1];
    vertices[13].sourceCoordinate[0] = radiusW * sourceSx1 + sourceTx1;
    vertices[13].sourceCoordinate[1] = sourceSy1 + sourceTy1;
    vertices[13].sourceCoordinate[2] = radiusW * sourceSx2 + sourceTx2;
    vertices[13].sourceCoordinate[3] = sourceSy2 + sourceTy2;
    vertices[13].backgroundColor = backgroundColorRow4;
    vertices[14].position[0] = width - radius;
    vertices[14].position[1] = height;
    vertices[14].shapeCoordinate[0] = shapeCoordinate[14][0];
    vertices[14].shapeCoordinate[1] = shapeCoordinate[14][1];
    vertices[14].sourceCoordinate[0] = (1.0f - radiusW) * sourceSx1 + sourceTx1;
    vertices[14].sourceCoordinate[1] = sourceSy1 + sourceTy1;
    vertices[14].sourceCoordinate[2] = (1.0f - radiusW) * sourceSx2 + sourceTx2;
    vertices[14].sourceCoordinate[3] = sourceSy2 + sourceTy2;
    vertices[14].backgroundColor = backgroundColorRow4;
    vertices[15].position[0] = width;
    vertices[15].position[1] = height;
    vertices[15].shapeCoordinate[0] = shapeCoordinate[15][0];
    vertices[15].shapeCoordinate[1] = shapeCoordinate[15][1];
    vertices[15].sourceCoordinate[0] = sourceSx1 + sourceTx1;
    vertices[15].sourceCoordinate[1] = sourceSy1 + sourceTy1;
    vertices[15].sourceCoordinate[2] = sourceSx2 + sourceTx2;
    vertices[15].sourceCoordinate[3] = sourceSy2 + sourceTy2;
    vertices[15].backgroundColor = backgroundColorRow4;

    markDirty(DirtyGeometry);
}

// --- QtQuick item ---

struct ShapeTextures
{
    ShapeTextures() : high(0), low(0) {}
    QSGTexture* high;
    QSGTexture* low;
};

static QHash<QOpenGLContext*, ShapeTextures> shapeTexturesHash;

const float implicitGridUnitWidth = 8.0f;
const float implicitGridUnitHeight = 8.0f;

// Threshold in grid unit defining the texture quality to be used.
const float lowHighTextureThreshold = 11.0f;

/*! \qmltype UbuntuShape
    \instantiates UCUbuntuShape
    \inqmlmodule Ubuntu.Components 1.1
    \ingroup ubuntu
    \brief The base graphical component of the Ubuntu UI Toolkit.

    The UbuntuShape is a rounded rectangle (based on a \l
    {https://en.wikipedia.org/wiki/Squircle}{squircle}) that can shape a set layers composed, from
    back to front, of a background color (solid or linear gradient), an optional source texture and
    a rectangle overlay. Different properties allow to change the look of the shape and its layers.

    Examples:

    \qml
        import Ubuntu.Components 1.1

        UbuntuShape {
            backgroundColor: "green"
        }
    \endqml

    \qml
        import Ubuntu.Components 1.1

        UbuntuShape {
            source: Image {
               source: "ubuntu.png"
           }
       }
    \endqml
*/
UCUbuntuShape::UCUbuntuShape(QQuickItem* parent)
    : QQuickItem(parent)
    , source_(NULL)
    , sourceTextureProvider_(NULL)
    , backgroundColor_(qRgba(0, 0, 0, 0))
    , secondaryBackgroundColor_(qRgba(0, 0, 0, 0))
    , borderSource_("radius_idle.sci")
    , sourceScale_(1.0f, 1.0f)
    , sourceTranslation_(0.0f, 0.0f)
    , sourceTransform_(1.0f, 1.0f, 0.0f, 0.0f)
    , overlayX_(0)
    , overlayY_(0)
    , overlayWidth_(0)
    , overlayHeight_(0)
    , overlayColor_(qRgba(0, 0, 0, 0))
    , radius_(SmallRadius)
    , border_(IdleBorder)
    , imageHorizontalAlignment_(AlignHCenter)
    , imageVerticalAlignment_(AlignVCenter)
    , backgroundMode_(SolidColor)
    , sourceHorizontalAlignment_(AlignHCenter)
    , sourceVerticalAlignment_(AlignVCenter)
    , sourceFillMode_(Stretch)
    , sourceHorizontalWrapMode_(Transparent)
    , sourceVerticalWrapMode_(Transparent)
    , sourceOpacity_(255)
    , flags_(Stretched)
{
    setFlag(ItemHasContents);
    QObject::connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()), this,
                     SLOT(gridUnitChanged()));
    const float gridUnit = UCUnits::instance().gridUnit();
    setImplicitWidth(implicitGridUnitWidth * gridUnit);
    setImplicitHeight(implicitGridUnitHeight * gridUnit);
    update();
}

/*! \qmlproperty string UbuntuShape::radius

    This property defines the corner radius. Two fixed values are supported: \c "small" and \c
    "medium". The default value is \c "small".
*/
void UCUbuntuShape::setRadius(const QString& radius)
{
    const Radius newRadius = (radius == "medium") ? MediumRadius : SmallRadius;
    if (radius_ != newRadius) {
        radius_ = newRadius;
        update();
        Q_EMIT radiusChanged();
    }
}

/*! \qmlproperty string UbuntuShape::borderSource

    This property defines the look of the shape borders. The supported strings are \c
    "radius_idle.sci" providing an idle button style and \c "radius_pressed.sci" providing a pressed
    button style. Any other strings (like the empty one \c "") disables styling. The default value
    is \c "radius_idle.sci".
*/
void UCUbuntuShape::setBorderSource(const QString& borderSource)
{
    if (borderSource_ != borderSource) {
        if (borderSource.endsWith(QString("radius_idle.sci"))) {
            border_ = IdleBorder;
        } else if (borderSource.endsWith(QString("radius_pressed.sci"))) {
            border_ = PressedBorder;
        } else {
            border_ = RawBorder;
        }
        // It's useless to store the string (and takes a few bytes) but we used to return the string
        // given by the user, so we can't remove it for compatibility reasons.
        borderSource_ = borderSource;
        update();
        Q_EMIT borderSourceChanged();
    }
}

/*! \qmlproperty variant UbuntuShape::source

    This property sets the source provider of a texture rendered in the UbuntuShape. Supported types
    are \c Image, \c AnimatedImage (to render a GIF image for instance) and \c ShaderEffectSource
    (to embed a UI previously rendered with QML). It is blended over the background color. The
    default value is \c null.

    It can be set either with an inlined \c Image:

    \qml
        Item {
            UbuntuShape {
                source: Image { source: "ubuntu.png" }
            }
        }
    \endqml

    or with an \c Image \c id:

    \qml
        Item {
            Image {
                id: img
                visible: false
                source: "ubuntu.png"
            }
            UbuntuShape {
                source: img
            }
        }
    \endqml

    Note that in this case, the \c Image is stored in the scene tree as any other items. So setting
    it as not visible might be needed.

    The \l{https://en.wikipedia.org/wiki/Texture_filtering}{sampling filter} is set through the
    given Item's \c smooth property. Set it to \c false for nearest-neighbor filtering or to \c true
    for bilinear filtering.

    The fill modes and alignments set on the \c Image and \c AnimatedImage are not monitored, use
    the appropriate UbuntuShape properties instead (\l sourceFillMode, \l sourceHorizontalAlignment
    and \l sourceVerticalAlignment).

    \note Setting this disables support for the deprecated \l image property.
*/
void UCUbuntuShape::setSource(const QVariant& source)
{
    dropImageSupport();

    QQuickItem* newSource = qobject_cast<QQuickItem*>(qvariant_cast<QObject*>(source));
    if (source_ != newSource) {
        if (newSource) {
            if (!newSource->parentItem()) {
                // Inlined images need a parent and must not be visible.
                newSource->setParentItem(this);
                newSource->setVisible(false);
            }
            flags_ |= DirtySourceTransform;
        }
        source_ = newSource;
        update();
        Q_EMIT sourceChanged();
    }
}

/*! \qmlproperty real UbuntuShape::sourceOpacity

    This property holds the opacity of the \l source texture. Opacity is specified as a number
    between 0.0 (fully transparent) and 1.0 (fully opaque). The default value is 1.0.

    \note Setting this disables support for the deprecated \l image property.
*/
void UCUbuntuShape::setSourceOpacity(float sourceOpacity)
{
    dropImageSupport();

    const quint8 sourceOpacityPacked =
        qMax(0.0f, qMin(1.0f, sourceOpacity)) * static_cast<float>(0xff);
    if (sourceOpacity_ != sourceOpacityPacked) {
        sourceOpacity_ = sourceOpacityPacked;
        update();
        Q_EMIT sourceOpacityChanged();
    }
}

/*! \qmlproperty enumeration UbuntuShape::sourceFillMode

    This properties defines how the \l source texture fills the UbuntuShape. The modes are the same
    as the ones used by \c Image, minus the tiling which is exposed through wrapping properties (\l
    sourceHorizontalWrapMode and \l sourceVerticalWrapMode). The default value is \c
    UbuntuShape.Stretch.

    \note Setting this disables support for the deprecated \l image property.

    \list
    \li \b UbuntuShape.Stretch - the source is scaled non-uniformly to fit
    \li \b UbuntuShape.PreserveAspectFit - the source is scaled uniformly to fit without cropping
    \li \b UbuntuShape.PreserveAspectCrop - the source is scaled uniformly to fit with cropping
    \li \b UbuntuShape.Pad - the source is not scaled
    \endlist
*/
void UCUbuntuShape::setSourceFillMode(UCUbuntuShape::FillMode sourceFillMode)
{
    dropImageSupport();

    if (sourceFillMode_ != sourceFillMode) {
        sourceFillMode_ = sourceFillMode;
        flags_ |= DirtySourceTransform;
        update();
        Q_EMIT sourceFillModeChanged();
    }
}

/*! \qmlproperty enumeration UbuntuShape::sourceHorizontalWrapMode
    \qmlproperty enumeration UbuntuShape::sourceVerticalWrapMode

    When the \l sourceFillMode is set to \c UbuntuShape.Pad or \c UbuntuShape.PreserveAspectFit or
    when the \l sourceScale and/or \l sourceTranslation properties are changed, the \l source
    texture might not cover the entire UbuntuShape area. This property defines how the source
    texture wraps outside of its content area. The default value is \c UbuntuShape.Transparent.

    Ensure \c UbuntuShape.Repeat is used only when needed (in case of an \c Image or \c
    AnimatedImage source) as it requires the underlying texture to be extracted from its atlas. That
    slows down the rendering speed since it prevents the QtQuick renderer to batch the UbuntuShape
    with others.

    \note Some OpenGL ES 2 implementations do not support \c UbuntuShape.Repeat with
    non-power-of-two sized source textures.
    \note Setting this disables support for the deprecated \l image property.

    \list
    \li \b UbuntuShape.Transparent - the source is clamped to transparent
    \li \b UbuntuShape.Repeat - the source is repeated indefinitely (not supported yet)
    \endlist
*/
void UCUbuntuShape::setSourceHorizontalWrapMode(UCUbuntuShape::WrapMode sourceHorizontalWrapMode)
{
    dropImageSupport();

    if (sourceHorizontalWrapMode_ != sourceHorizontalWrapMode) {
        sourceHorizontalWrapMode_ = sourceHorizontalWrapMode;
        update();
        Q_EMIT sourceHorizontalWrapModeChanged();
    }
}

void UCUbuntuShape::setSourceVerticalWrapMode(UCUbuntuShape::WrapMode sourceVerticalWrapMode)
{
    dropImageSupport();

    if (sourceVerticalWrapMode_ != sourceVerticalWrapMode) {
        sourceVerticalWrapMode_ = sourceVerticalWrapMode;
        update();
        Q_EMIT sourceVerticalWrapModeChanged();
    }
}

/*! \qmlproperty enumeration UbuntuShape::sourceHorizontalAlignment

    This property defines how the \l source texture is horizontally aligned inside the UbuntuShape
    area. The default value is \c UbuntuShape.AlignLeft.

    \note Setting this disables support for the deprecated \l image property.

    \list
    \li \b UbuntuShape.AlignLeft - the source is aligned to the left
    \li \b UbuntuShape.AlignHCenter - the source is aligned to the horizontal center
    \li \b UbuntuShape.AlignRight - the source is aligned to the right
    \endlist
*/
void UCUbuntuShape::setSourceHorizontalAlignment(
    UCUbuntuShape::HAlignment sourceHorizontalAlignment)
{
    dropImageSupport();

    if (sourceHorizontalAlignment_ != sourceHorizontalAlignment) {
        sourceHorizontalAlignment_ = sourceHorizontalAlignment;
        flags_ |= DirtySourceTransform;
        update();
        Q_EMIT sourceHorizontalAlignmentChanged();
    }
}

/*! \qmlproperty enumeration UbuntuShape::sourceVerticalAlignment

    This property defines how the \l source texture is vertically aligned inside the UbuntuShape
    area. The default value is \c UbuntuShape.AlignTop.

    \note Setting this disables support for the deprecated \l image property.

    \list
    \li \b UbuntuShape.AlignTop - the source is aligned to the top
    \li \b UbuntuShape.AlignVCenter - the source is aligned to the vertical center
    \li \b UbuntuShape.AlignBottom - the source is aligned to the bottom
    \endlist
*/
void UCUbuntuShape::setSourceVerticalAlignment(UCUbuntuShape::VAlignment sourceVerticalAlignment)
{
    dropImageSupport();

    if (sourceVerticalAlignment_ != sourceVerticalAlignment) {
        sourceVerticalAlignment_ = sourceVerticalAlignment;
        flags_ |= DirtySourceTransform;
        update();
        Q_EMIT sourceVerticalAlignmentChanged();
    }
}

/*! \qmlproperty vector2d UbuntuShape::sourceTranslation

    This property defines the two-component vector in normalized item coordinates used to translate
    the \l source texture. The default value is \c {Qt.vector2d(0.0,0.0)}.

    That can be used to put the \l source texture at a precise position, to create infinite
    scrolling animations (using the \c UbuntuShape.Repeat wrap mode), etc.

    \note Setting this disables support for the deprecated \l image property.
*/
void UCUbuntuShape::setSourceTranslation(const QVector2D& sourceTranslation)
{
    dropImageSupport();

    if (sourceTranslation_ != sourceTranslation) {
        sourceTranslation_ = sourceTranslation;
        flags_ |= DirtySourceTransform;
        update();
        Q_EMIT sourceTranslationChanged();
    }
}

/*! \qmlproperty vector2d UbuntuShape::sourceScale

    This property defines the two-component vector used to scale the \l source texture. The texture
    is scaled at the alignment point defined by \l sourceHorizontalAlignment and \l
    sourceVerticalAlignment. The default value is \c {Qt.vector2d(1.0,1.0)}.

    That can be used to change the size of the \l source texture, to flip it horizontally and/or
    vertically, to achieve pulsing animations, etc.

    Here is a code sample showing how to apply an horizontal flip:

    \qml
        UbuntuShape {
            source: Image { source: "ubuntu.png" }
            sourceScale: Qt.vector2d(-1.0, 1.0)
        }
    \endqml

    \note Setting this disables support for the deprecated \l image property.
*/
void UCUbuntuShape::setSourceScale(const QVector2D& sourceScale)
{
    dropImageSupport();

    if (sourceScale_ != sourceScale) {
        sourceScale_ = sourceScale;
        flags_ |= DirtySourceTransform;
        update();
        Q_EMIT sourceScaleChanged();
    }
}

// Deprecation layer.
void UCUbuntuShape::dropColorSupport()
{
    if (!(flags_ & BackgroundApiSet)) {
        flags_ |= BackgroundApiSet;
        backgroundColor_ = qRgba(0, 0, 0, 0);
        secondaryBackgroundColor_ = qRgba(0, 0, 0, 0);
        if (backgroundColor_) {
            Q_EMIT colorChanged();
        }
        if (secondaryBackgroundColor_) {
            Q_EMIT gradientColorChanged();
        }
    }
}

/*! \qmlproperty color UbuntuShape::backgroundColor

    This property defines the background color. The default value is transparent black.

    \note Setting this disables support for the deprecated \l color and \l gradientColor properties.
*/
void UCUbuntuShape::setBackgroundColor(const QColor& backgroundColor)
{
    dropColorSupport();

    const QRgb backgroundColorRgb = qRgba(
        backgroundColor.red(), backgroundColor.green(), backgroundColor.blue(),
        backgroundColor.alpha());
    if (backgroundColor_ != backgroundColorRgb) {
        backgroundColor_ = backgroundColorRgb;
        update();
        Q_EMIT backgroundColorChanged();
    }
}

/*! \qmlproperty color UbuntuShape::secondaryBackgroundColor

    This property defines the secondary background color. It is used when \l backgroundMode is set
    to \c UbuntuShape.VerticalGradient. The default value is transparent black.

    \note Setting this disables support for the deprecated \l color and \l gradientColor properties.
*/
void UCUbuntuShape::setSecondaryBackgroundColor(const QColor& secondaryBackgroundColor)
{
    dropColorSupport();

    const QRgb secondaryBackgroundColorRgb = qRgba(
        secondaryBackgroundColor.red(), secondaryBackgroundColor.green(),
        secondaryBackgroundColor.blue(), secondaryBackgroundColor.alpha());
    if (secondaryBackgroundColor_ != secondaryBackgroundColorRgb) {
        secondaryBackgroundColor_ = secondaryBackgroundColorRgb;
        update();
        Q_EMIT secondaryBackgroundColorChanged();
    }
}

/*! \qmlproperty enumeration UbuntuShape::backgroundMode

    This property defines the background rendering mode. The default value is \c
    UbuntuShape.SolidColor.

    \note Setting this disables support for the deprecated \l color and \l gradientColor properties.

    \list
    \li \b UbuntuShape.SolidColor - the color is \l backgroundColor
    \li \b UbuntuShape.VerticalGradient - the color is a vertical gradient from \l backgroundColor
           to \l secondaryBackgroundColor.
    \endlist
*/
void UCUbuntuShape::setBackgroundMode(BackgroundMode backgroundMode)
{
    dropColorSupport();

    if (backgroundMode_ != backgroundMode) {
        backgroundMode_ = backgroundMode;
        update();
        Q_EMIT backgroundModeChanged();
    }
}

/*! \qmlproperty rect UbuntuShape::overlayGeometry

    This property sets the overlay rectangle. The default value is the empty rectangle.

    It is defined by a position and a size in normalized item coordinates (in the range between 0
    and 1) with the origin at the top-left corner. An overlay covering the bottom part and starting
    at the vertical center can be done like that:

    \qml
        UbuntuShape {
            width: 200; height: 200
            overlayColor: Qt.rgba(0.0, 0.0, 0.5, 0.5)
            overlayGeometry: Qt.rect(0.0, 0.5, 1.0, 0.5)
        }
    \endqml

    Converting a position and a size in pixels to normalized item coordinates can be done with a
    division by the size. Here is an equivalent of the previous code sample:

    \qml
        UbuntuShape {
            width: 200; height: 200
            overlayColor: Qt.rgba(0.0, 0.0, 0.5, 0.5)
            overlayGeometry: Qt.rect(100.0/width, 100.0/height, 200.0/width, 100.0/height)
        }
    \endqml

    A geometry exceeding the item area is cropped.
*/
void UCUbuntuShape::setOverlayGeometry(const QRectF& overlayGeometry)
{
    // Crop rectangle and pack to 16-bit unsigned integers.
    const float x = qMax(0.0f, qMin(1.0f, static_cast<float>(overlayGeometry.x())));
    float width = qMax(0.0f, static_cast<float>(overlayGeometry.width()));
    if ((x + width) > 1.0f) {
        width += 1.0f - (x + width);
    }
    const float y = qMax(0.0f, qMin(1.0f, static_cast<float>(overlayGeometry.y())));
    float height = qMax(0.0f, static_cast<float>(overlayGeometry.height()));
    if ((y + height) > 1.0f) {
        height += 1.0f - (y + height);
    }
    const quint16 overlayX = static_cast<quint16>(x * 0xffff);
    const quint16 overlayY = static_cast<quint16>(y * 0xffff);
    const quint16 overlayWidth = static_cast<quint16>(width * 0xffff);
    const quint16 overlayHeight = static_cast<quint16>(height * 0xffff);

    if ((overlayX_ != overlayX) || (overlayY_ != overlayY) ||
        (overlayWidth_ != overlayWidth) || (overlayHeight_ != overlayHeight)) {
        overlayX_ = overlayX;
        overlayY_ = overlayY;
        overlayWidth_ = overlayWidth;
        overlayHeight_ = overlayHeight;
        update();
        Q_EMIT overlayGeometryChanged();
    }
}

/*! \qmlproperty color UbuntuShape::overlayColor

    This property sets the color of the overlay rectangle defined by \l overlayGeometry. The default
    value is transparent black.
*/
void UCUbuntuShape::setOverlayColor(const QColor& overlayColor)
{
    const QRgb overlayColorRgb = qRgba(
        overlayColor.red(), overlayColor.green(), overlayColor.blue(), overlayColor.alpha());
    if (overlayColor_ != overlayColorRgb) {
        overlayColor_ = overlayColorRgb;
        update();
        Q_EMIT overlayColorChanged();
    }
}

/*! \qmlproperty color UbuntuShape::color
    \deprecated

    This property defines the color used to fill the UbuntuShape when there is no \l image set. If
    \l gradientColor is set, this property defines the top color of the gradient. The default value
    is transparent black.

    \note Use \l backgroundColor, \l secondaryBackgroundColor and \l backgroundMode instead.
*/
void UCUbuntuShape::setColor(const QColor& color)
{
    if (!(flags_ & BackgroundApiSet)) {
        const QRgb colorRgb = qRgba(color.red(), color.green(), color.blue(), color.alpha());
        if (backgroundColor_ != colorRgb) {
            backgroundColor_ = colorRgb;
            // gradientColor has the same value as color unless it was explicitly set.
            if (!(flags_ & GradientColorSet)) {
                secondaryBackgroundColor_ = colorRgb;
                Q_EMIT gradientColorChanged();
            }
            update();
            Q_EMIT colorChanged();
        }
    }
}

/*! \deprecated
    \qmlproperty color UbuntuShape::gradientColor

    This property defines the bottom color used for the vertical gradient filling the UbuntuShape
    when there is no \image set. As long as this property is not set, a single color (defined by \l
    color) is used to fill the UbuntuShape.

    \note Use \l backgroundColor, \l secondaryBackgroundColor and \l backgroundMode instead.
*/
void UCUbuntuShape::setGradientColor(const QColor& gradientColor)
{
    if (!(flags_ & BackgroundApiSet)) {
        flags_ |= GradientColorSet;
        const QRgb gradientColorRgb = qRgba(
            gradientColor.red(), gradientColor.green(), gradientColor.blue(),
            gradientColor.alpha());
        if (secondaryBackgroundColor_ != gradientColorRgb) {
            secondaryBackgroundColor_ = gradientColorRgb;
            update();
            Q_EMIT gradientColorChanged();
        }
    }
}

/*! \deprecated
    \qmlproperty Image UbuntuShape::image

    This property holds the \c Image or \c ShaderEffectSource rendered in the UbuntuShape. In case
    of an \c Image, it watches for fillMode (\c Image.Stretch and\c Image.PreserveAspectCrop), \c
    horizontalAlignment and \c verticalAlignment property changes. The default value is \c null.

    \note Use \l source instead.
*/
void UCUbuntuShape::setImage(const QVariant& image)
{
    if (!(flags_ & SourceApiSet)) {
        QQuickItem* newImage = qobject_cast<QQuickItem*>(qvariant_cast<QObject*>(image));
        if (source_ != newImage) {
            if (newImage) {
                // Watch for property changes.
                updateFromImageProperties(newImage);
                connectToImageProperties(newImage);
                if (!newImage->parentItem()) {
                    // Inlined images need a parent and must not be visible.
                    newImage->setParentItem(this);
                    newImage->setVisible(false);
                }
                flags_ |= DirtySourceTransform;
            }
            QObject::disconnect(source_);
            update();
            source_ = newImage;
            Q_EMIT imageChanged();
        }
    }
}

// Deprecation layer. Even though "stretched" is exposed as a QML property, it's only been used when
// there was a QML UbuntuShape proxy. This is why we don't provide doc for it. We'll still have to
// maintain it for a while for compatibility reasons.
void UCUbuntuShape::setStretched(bool stretched)
{
    if (!(flags_ & SourceApiSet)) {
        if (!!(flags_ & Stretched) != stretched) {
            if (stretched) {
                flags_ |= Stretched;
            } else {
                flags_ &= ~Stretched;
            }
            flags_ |= DirtySourceTransform;
            update();
            Q_EMIT stretchedChanged();
        }
    }
}

// Deprecation layer. Same comment as setStretched().
void UCUbuntuShape::setHorizontalAlignment(HAlignment horizontalAlignment)
{
    if (!(flags_ & SourceApiSet)) {
        if (imageHorizontalAlignment_ != horizontalAlignment) {
            imageHorizontalAlignment_ = horizontalAlignment;
            flags_ |= DirtySourceTransform;
            update();
            Q_EMIT horizontalAlignmentChanged();
        }
    }
}

// Deprecation layer. Same comment as setStretched().
void UCUbuntuShape::setVerticalAlignment(VAlignment verticalAlignment)
{
    if (!(flags_ & SourceApiSet)) {
        if (imageVerticalAlignment_ != verticalAlignment) {
            imageVerticalAlignment_ = verticalAlignment;
            flags_ |= DirtySourceTransform;
            update();
            Q_EMIT verticalAlignmentChanged();
        }
    }
}

// Deprecation layer.
void UCUbuntuShape::updateFromImageProperties(QQuickItem* image)
{
    int alignment;

    // UbuntuShape::stretched depends on Image::fillMode.
    QQuickImage::FillMode fillMode = (QQuickImage::FillMode)image->property("fillMode").toInt();
    if (fillMode == QQuickImage::PreserveAspectCrop) {
        setStretched(false);
    } else {
        setStretched(true);
    }

    // UbuntuShape::horizontalAlignment depends on Image::horizontalAlignment.
    int imageHorizontalAlignment = image->property("horizontalAlignment").toInt();
    if (imageHorizontalAlignment == Qt::AlignLeft) {
        alignment = AlignLeft;
    } else if (imageHorizontalAlignment == Qt::AlignRight) {
        alignment = AlignRight;
    } else {
        alignment = AlignHCenter;
    }
    setHorizontalAlignment(static_cast<HAlignment>(alignment));

    // UbuntuShape::verticalAlignment depends on Image::verticalAlignment.
    int imageVerticalAlignment = image->property("verticalAlignment").toInt();
    if (imageVerticalAlignment == Qt::AlignTop) {
        alignment = AlignTop;
    } else if (imageVerticalAlignment == Qt::AlignBottom) {
        alignment = AlignBottom;
    } else {
        alignment = AlignVCenter;
    }
    setVerticalAlignment(static_cast<UCUbuntuShape::VAlignment>(alignment));
}

// Deprecation layer.
void UCUbuntuShape::connectToPropertyChange(QObject* sender, const char* property,
                                            QObject* receiver, const char* slot)
{
    int propertyIndex = sender->metaObject()->indexOfProperty(property);
    if (propertyIndex != -1) {
        QMetaMethod changeSignal = sender->metaObject()->property(propertyIndex).notifySignal();
        int slotIndex = receiver->metaObject()->indexOfSlot(slot);
        QMetaMethod updateSlot = receiver->metaObject()->method(slotIndex);
        QObject::connect(sender, changeSignal, receiver, updateSlot);
    }
}

// Deprecation layer.
void UCUbuntuShape::connectToImageProperties(QQuickItem* image)
{
    connectToPropertyChange(image, "fillMode", this, "imagePropertiesChanged()");
    connectToPropertyChange(image, "horizontalAlignment", this, "imagePropertiesChanged()");
    connectToPropertyChange(image, "verticalAlignment", this, "imagePropertiesChanged()");
}

// Deprecation layer.
void UCUbuntuShape::imagePropertiesChanged()
{
    QQuickItem* image = qobject_cast<QQuickItem*>(sender());
    updateFromImageProperties(image);
}

// Deprecation layer.
void UCUbuntuShape::dropImageSupport()
{
    if (!(flags_ & SourceApiSet)) {
        flags_ |= SourceApiSet;
        if (source_) {
            QObject::disconnect(source_);
            source_ = NULL;
            update();
            Q_EMIT imageChanged();
        }
    }
}

void UCUbuntuShape::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    flags_ |= DirtySourceTransform;
}

void UCUbuntuShape::openglContextDestroyed()
{
    QOpenGLContext* context = qobject_cast<QOpenGLContext*>(sender());
    if (context) {
        QHash<QOpenGLContext*, ShapeTextures>::iterator it = shapeTexturesHash.find(context);
        if (it != shapeTexturesHash.end()) {
            ShapeTextures &shapeTextures = it.value();
            delete shapeTextures.high;
            delete shapeTextures.low;
            shapeTexturesHash.erase(it);
        }
    }
}

void UCUbuntuShape::gridUnitChanged()
{
    const float gridUnit = UCUnits::instance().gridUnit();
    setImplicitWidth(implicitGridUnitWidth * gridUnit);
    setImplicitHeight(implicitGridUnitHeight * gridUnit);
    update();
}

void UCUbuntuShape::providerDestroyed(QObject* object)
{
    Q_UNUSED(object);
    sourceTextureProvider_ = NULL;
}

void UCUbuntuShape::textureChanged()
{
    flags_ |= DirtySourceTransform;
    update();
}

// Gets the nearest boundary to coord in the texel grid of the given size.
static Q_DECL_CONSTEXPR float roundTextureCoord(float coord, float size)
{
    return roundf(coord * size) / size;
}

void UCUbuntuShape::updateSourceTransform(float itemWidth, float itemHeight, FillMode fillMode,
                                          HAlignment horizontalAlignment,
                                          VAlignment verticalAlignment, const QSize& textureSize)
{
    float fillSx, fillSy;
    if (fillMode == PreserveAspectFit) {
        const float textureRatio = static_cast<float>(textureSize.width()) / textureSize.height();
        const float itemRatio = itemWidth / itemHeight;
        fillSx = (textureRatio < itemRatio) ? (itemRatio / textureRatio) : 1.0f;
        fillSy = (textureRatio < itemRatio) ? 1.0f : (textureRatio / itemRatio);
    } else if (fillMode == PreserveAspectCrop) {
        const float textureRatio = static_cast<float>(textureSize.width()) / textureSize.height();
        const float itemRatio = itemWidth / itemHeight;
        fillSx = (textureRatio < itemRatio) ? 1.0f : (itemRatio / textureRatio);
        fillSy = (textureRatio < itemRatio) ? (textureRatio / itemRatio) : 1.0f;
    } else if (fillMode == Pad) {
        fillSx = itemWidth / textureSize.width();
        fillSy = itemHeight / textureSize.height();
    } else {
        fillSx = 1.0f;
        fillSy = 1.0f;
    }

    const float sourceSxInv = 1.0f / sourceScale_.x();
    const float sourceSyInv = 1.0f / sourceScale_.y();
    // Multiplied by fillS* so that the translation unit is in normalized item coordinates.
    const float sourceTx = (sourceTranslation_.x() * sourceSxInv) * fillSx;
    const float sourceTy = (sourceTranslation_.y() * sourceSyInv) * fillSy;
    const float sx = fillSx * sourceSxInv;
    const float sy = fillSy * sourceSyInv;
    const float factorTable[3] = { 0.0f, 0.5f, 1.0f };
    const float hFactor = factorTable[static_cast<int>(horizontalAlignment)];
    const float vFactor = factorTable[static_cast<int>(verticalAlignment)];
    const float tx = hFactor * (1.0f - sx) - sourceTx;
    const float ty = vFactor * (1.0f - sy) - sourceTy;

    // Rounding is important to get padded texture perfectly mapped to the pixel grid. It shouldn't
    // be necessary when there's a scaling but we make it consistent by applying the scale factors
    // to the texture size, so that there's no ugly position jumps with big scaling values.
    sourceTransform_ = QVector4D(
        sx, sy, roundTextureCoord(tx, textureSize.width() * sourceScale_.x()),
        roundTextureCoord(ty, textureSize.height() * sourceScale_.y()));
}

QSGNode* UCUbuntuShape::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data)
{
    Q_UNUSED(data);

    const QSizeF itemSize = QSize(width(), height());

    if (itemSize.isEmpty()) {
        delete oldNode;
        return NULL;
    }

    // OpenGL allocates textures per context, so we store textures reused by all shape instances
    // per context as well.
    QOpenGLContext* openglContext = window() ? window()->openglContext() : NULL;
    if (!openglContext) {
        qCritical() << "Window has no OpenGL context!";
        delete oldNode;
        return NULL;
    }
    ShapeTextures &shapeTextures = shapeTexturesHash[openglContext];
    if (!shapeTextures.high) {
        shapeTextures.high = window()->createTextureFromImage(
            QImage(shapeTextureHigh.data, shapeTextureHigh.width, shapeTextureHigh.height,
                   QImage::Format_ARGB32_Premultiplied));
        shapeTextures.low = window()->createTextureFromImage(
            QImage(shapeTextureLow.data, shapeTextureLow.width, shapeTextureLow.height,
                   QImage::Format_ARGB32_Premultiplied));
        QObject::connect(openglContext, SIGNAL(aboutToBeDestroyed()),
                         this, SLOT(openglContextDestroyed()), Qt::DirectConnection);
    }

    ShapeNode* node = static_cast<ShapeNode*>(oldNode);
    if (!node) {
        node = new ShapeNode(this);
    }

    // Get the texture info and update the source transform if needed.
    QSGTextureProvider* provider = source_ ? source_->textureProvider() : NULL;
    const QSGTexture* texture = provider ? provider->texture() : NULL;
    QRectF textureRect(0.0f, 0.0f, 1.0f, 1.0f);
    if (texture) {
        if (sourceHorizontalWrapMode_ == Transparent && sourceVerticalWrapMode_ == Transparent) {
            textureRect = texture->normalizedTextureSubRect();
        }
        if (flags_ & DirtySourceTransform) {
            if (flags_ & SourceApiSet) {
                updateSourceTransform(itemSize.width(), itemSize.height(), sourceFillMode_,
                                      sourceHorizontalAlignment_, sourceVerticalAlignment_,
                                      texture->textureSize());
            } else {
                FillMode imageFillMode = (flags_ & Stretched) ? Stretch : PreserveAspectCrop;
                updateSourceTransform(itemSize.width(), itemSize.height(), imageFillMode,
                                      imageHorizontalAlignment_, imageVerticalAlignment_,
                                      texture->textureSize());
            }
            flags_ &= ~DirtySourceTransform;
        }
    }

    // Update the shape item whenever the source item's texture changes.
    if (provider != sourceTextureProvider_) {
        if (sourceTextureProvider_) {
            QObject::disconnect(sourceTextureProvider_, SIGNAL(textureChanged()),
                                this, SLOT(textureChanged()));
            QObject::disconnect(sourceTextureProvider_, SIGNAL(destroyed()),
                                this, SLOT(providerDestroyed()));
        }
        if (provider) {
            QObject::connect(provider, SIGNAL(textureChanged()), this, SLOT(textureChanged()));
            QObject::connect(provider, SIGNAL(destroyed()), this, SLOT(providerDestroyed()));
        }
        sourceTextureProvider_ = provider;
    }

    ShapeMaterial::Data* materialData = node->material()->data();
    const float gridUnit = UCUnits::instance().gridUnit();

    ShapeTextureData* shapeTextureData;
    if (gridUnit > lowHighTextureThreshold) {
        shapeTextureData = &shapeTextureHigh;
        materialData->shapeTexture = shapeTextures.high;
    } else {
        shapeTextureData = &shapeTextureLow;
        materialData->shapeTexture = shapeTextures.low;
    }

    // Set the shape texture to be used by the materials depending on current grid unit. The radius
    // is set considering the current grid unit and the texture raster grid unit. When the item size
    // is less than 2 radii, the radius is scaled down.
    float radius = (radius_ == SmallRadius) ?
        shapeTextureData->smallRadius : shapeTextureData->mediumRadius;
    const float scaleFactor = gridUnit / shapeTextureData->gridUnit;
    materialData->shapeTextureFiltering = QSGTexture::Nearest;
    if (scaleFactor != 1.0f) {
        radius *= scaleFactor;
        materialData->shapeTextureFiltering = QSGTexture::Linear;
    }
    const float halfMinWidthHeight = qMin(itemSize.width(), itemSize.height()) * 0.5f;
    if (radius > halfMinWidthHeight) {
        radius = halfMinWidthHeight;
        materialData->shapeTextureFiltering = QSGTexture::Linear;
    }

    quint8 flags = 0;

    // Update source material data.
    if (texture && sourceOpacity_) {
        materialData->sourceTextureProvider = sourceTextureProvider_;
        materialData->sourceOpacity = sourceOpacity_;
        if (sourceHorizontalWrapMode_ == Repeat) {
            flags |= ShapeMaterial::Data::HorizontallyRepeated;
        }
        if (sourceVerticalWrapMode_ == Repeat) {
            flags |= ShapeMaterial::Data::VerticallyRepeated;
        }
        flags |= ShapeMaterial::Data::Textured;
    } else {
        materialData->sourceTextureProvider = NULL;
        materialData->sourceOpacity = 0;
    }

    // Update overlay material data.
    if ((overlayWidth_ != 0) && (overlayHeight_ != 0)) {
        const quint32 a = qAlpha(overlayColor_);
        materialData->overlayColor = qRgba(
            (qRed(overlayColor_) * a) / 255, (qGreen(overlayColor_) * a) / 255,
            (qBlue(overlayColor_) * a) / 255, a);
        materialData->overlaySteps[0] = overlayX_;
        materialData->overlaySteps[1] = overlayY_;
        materialData->overlaySteps[2] = overlayX_ + overlayWidth_;
        materialData->overlaySteps[3] = overlayY_ + overlayHeight_;
        flags |= ShapeMaterial::Data::Overlaid;
    } else {
        // Overlay data has to be set to 0 so that shapes with different values can be batched
        // together (ShapeMaterial::compare() uses memcmp()).
        materialData->overlayColor = qRgba(0, 0, 0, 0);
        materialData->overlaySteps[0] = 0;
        materialData->overlaySteps[1] = 0;
        materialData->overlaySteps[2] = 0;
        materialData->overlaySteps[3] = 0;
    }

    materialData->flags = flags;

    // Select the right shape texture coordinates.
    int index = (border_ == RawBorder) ? 0 : (border_ == IdleBorder) ? 1 : 2;
    if (radius_ == SmallRadius) {
        index += 3;
    }

    // Select the background color.
    QRgb backgroundColor, secondaryBackgroundColor;
    if (flags_ & BackgroundApiSet) {
        backgroundColor = backgroundColor_;
        secondaryBackgroundColor =
            (backgroundMode_ == SolidColor) ? backgroundColor_ : secondaryBackgroundColor_;
    } else {
        if (!texture) {
            backgroundColor = backgroundColor_;
            // For API compatibility reasons, secondaryBackgroundColor_ is set to backgroundColor_
            // as long as setGradientColor() isn't called, so we can safely set it here.
            secondaryBackgroundColor = secondaryBackgroundColor_;
        } else {
            // The deprecated image API was created such that whenever an image is set, the
            // background color is transparent.
            backgroundColor = qRgba(0, 0, 0, 0);
            secondaryBackgroundColor = qRgba(0, 0, 0, 0);
        }
    }

    node->setVertices(itemSize.width(), itemSize.height(), radius,
                      shapeTextureData->coordinate[index], sourceHorizontalWrapMode_,
                      sourceVerticalWrapMode_, sourceTransform_, textureRect, backgroundColor,
                      secondaryBackgroundColor);

    return node;
}
