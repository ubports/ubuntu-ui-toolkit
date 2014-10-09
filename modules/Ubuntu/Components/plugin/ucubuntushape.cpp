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

class ShapeMaterial : public QSGMaterial
{
public:
    struct Data
    {
        enum { ColoredFlag = (1 << 0) };
        QSGTexture* shapeTexture;
        QSGTextureProvider* imageTextureProvider;
        QRgb color;
        QRgb gradientColor;
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
    // by the UbuntuShape implementation and makes it easier to maintain.
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
    QOpenGLFunctions* glFuncs_;
    int matrixId_;
    int opacityId_;
    int color1Id_;
    int color2Id_;
    int coloredId_;
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
        "positionAttrib", "shapeCoordAttrib", "imageCoordAttrib", 0
    };
    return attributes;
}

void ShapeShader::initialize()
{
    QSGMaterialShader::initialize();
    program()->bind();
    program()->setUniformValue("shapeTexture", 0);
    program()->setUniformValue("imageTexture", 1);
    glFuncs_ = QOpenGLContext::currentContext()->functions();
    matrixId_ = program()->uniformLocation("matrix");
    opacityId_ = program()->uniformLocation("opacity");
    color1Id_ = program()->uniformLocation("color1");
    color2Id_ = program()->uniformLocation("color2");
    coloredId_ = program()->uniformLocation("colored");
}

void ShapeShader::updateState(const RenderState& state, QSGMaterial* newEffect,
                              QSGMaterial* oldEffect)
{
    Q_UNUSED(oldEffect);
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

    if (data->flags & ShapeMaterial::Data::ColoredFlag) {
        // Bind color uniforms.
        const float inv255 = 1.0f / 255.0f;
        QRgb c = data->color;
        program()->setUniformValue(color1Id_, QVector4D(
            qRed(c) * inv255, qGreen(c) * inv255, qBlue(c) * inv255, qAlpha(c) * inv255));
        c = data->gradientColor;
        program()->setUniformValue(color2Id_, QVector4D(
            qRed(c) * inv255, qGreen(c) * inv255, qBlue(c) * inv255, qAlpha(c) * inv255));
        program()->setUniformValue(coloredId_, true);
    } else {
        // Bind image texture and uniform.
        glFuncs_->glActiveTexture(GL_TEXTURE1);
        QSGTextureProvider* provider = data->imageTextureProvider;
        QSGTexture* texture = provider ? provider->texture() : NULL;
        if (texture) {
            texture->bind();
        } else {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        glFuncs_->glActiveTexture(GL_TEXTURE0);
        program()->setUniformValue(coloredId_, false);
    }

    // Bind QtQuick engine uniforms.
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
    return memcmp(&data_, otherData, sizeof(ShapeMaterial::Data));
}

// --- Scene graph node ---

class ShapeNode : public QSGGeometryNode
{
public:
    struct Vertex {
        float position[2];
        float shapeCoordinate[2];
        float imageCoordinate[2];
        float padding[2];  // Ensure a 32 bytes stride.
    };

    ShapeNode(UCUbuntuShape* item);
    ShapeMaterial* material() { return &material_; }
    void setVertices(const QRectF& geometry, float radius, QQuickItem* image, bool stretched,
                     UCUbuntuShape::HAlignment hAlignment,
                     UCUbuntuShape::VAlignment vAlignment, float shapeCoordinate[][2]);

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
    int indexCount;       // Number of indices.
    int vertexCount;      // Number of vertices.
    int attributeCount;   // Number of attributes.
    int stride;           // Offset in bytes from one vertex to the other.
    int positionCount;    // Number of components per position.
    int positionType;     // OpenGL type of the position components.
    int shapeCoordCount;  // Number of components per shape texture coordinate.
    int shapeCoordType;   // OpenGL type of the shape texture coordinate components.
    int imageCoordCount;  // Number of components per image texture coordinate.
    int imageCoordType;   // OpenGL type of the image texture coordinate components.
    int indexType;        // OpenGL type of the indices.
} shapeMesh = {
    shapeMeshIndices, ARRAY_SIZE(shapeMeshIndices),
    16, 3, sizeof(ShapeNode::Vertex), 2, GL_FLOAT, 2, GL_FLOAT, 2, GL_FLOAT, GL_UNSIGNED_SHORT
};

static const QSGGeometry::AttributeSet& getAttributes()
{
    static QSGGeometry::Attribute data[] = {
        QSGGeometry::Attribute::create(0, shapeMesh.positionCount, shapeMesh.positionType, true),
        QSGGeometry::Attribute::create(1, shapeMesh.shapeCoordCount, shapeMesh.shapeCoordType),
        QSGGeometry::Attribute::create(2, shapeMesh.imageCoordCount, shapeMesh.imageCoordType)
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

void ShapeNode::setVertices(const QRectF& geometry, float radius, QQuickItem* image, bool stretched,
                            UCUbuntuShape::HAlignment hAlignment,
                            UCUbuntuShape::VAlignment vAlignment, float shapeCoordinate[][2])
{
    ShapeNode::Vertex* vertices = reinterpret_cast<ShapeNode::Vertex*>(geometry_.vertexData());
    const QSGTextureProvider* provider = image ? image->textureProvider() : NULL;
    const QSGTexture* texture = provider ? provider->texture() : NULL;
    const float width = geometry.width();
    const float height = geometry.height();
    float topCoordinate;
    float bottomCoordinate;
    float leftCoordinate;
    float rightCoordinate;
    float radiusCoordinateWidth;
    float radiusCoordinateHeight;

    // FIXME(loicm) With a NxM image, a preserve aspect crop fill mode and a width
    //     component size of N (or a height component size of M), changing the the
    //     height (or width) breaks the 1:1 texel/pixel mapping for odd values.
    if (!stretched && texture) {
        // Preserve source image aspect ratio cropping areas exceeding destination rectangle.
        const float factors[3] = { 0.0f, 0.5f, 1.0f };
        const QSize srcSize = texture->textureSize();
        const float srcRatio = static_cast<float>(srcSize.width()) / srcSize.height();
        const float dstRatio = static_cast<float>(width) / height;
        if (dstRatio <= srcRatio) {
            const float inCoordinateSize = dstRatio / srcRatio;
            const float outCoordinateSize = 1.0f - inCoordinateSize;
            topCoordinate = 0.0f;
            bottomCoordinate = 1.0f;
            leftCoordinate = outCoordinateSize * factors[hAlignment];
            rightCoordinate = 1.0f - (outCoordinateSize * (1.0f - factors[hAlignment]));
            radiusCoordinateHeight = radius / height;
            radiusCoordinateWidth = (radius / width) * inCoordinateSize;
        } else {
            const float inCoordinateSize = srcRatio / dstRatio;
            const float outCoordinateSize = 1.0f - inCoordinateSize;
            topCoordinate = outCoordinateSize * factors[vAlignment];
            bottomCoordinate = 1.0f - (outCoordinateSize * (1.0f - factors[vAlignment]));
            leftCoordinate = 0.0f;
            rightCoordinate = 1.0f;
            radiusCoordinateHeight = (radius / height) * inCoordinateSize;
            radiusCoordinateWidth = radius / width;
        }
    } else {
        // Don't preserve source image aspect ratio stretching it in destination rectangle.
        topCoordinate = 0.0f;
        bottomCoordinate = 1.0f;
        leftCoordinate = 0.0f;
        rightCoordinate = 1.0f;
        radiusCoordinateHeight = radius / height;
        radiusCoordinateWidth = radius / width;
    }

    // Scale and translate coordinates of textures packed in an atlas.
    if (texture && texture->isAtlasTexture()) {
        const QRectF srcSubRect = texture->normalizedTextureSubRect();
        topCoordinate = topCoordinate * srcSubRect.height() + srcSubRect.y();
        bottomCoordinate = bottomCoordinate * srcSubRect.height() + srcSubRect.y();
        leftCoordinate = leftCoordinate * srcSubRect.width() + srcSubRect.x();
        rightCoordinate = rightCoordinate * srcSubRect.width() + srcSubRect.x();
        radiusCoordinateHeight = radiusCoordinateHeight * srcSubRect.height();
        radiusCoordinateWidth = radiusCoordinateWidth * srcSubRect.width();
    }

    // Set top row of 4 vertices.
    vertices[0].position[0] = 0.0f;
    vertices[0].position[1] = 0.0f;
    vertices[0].shapeCoordinate[0] = shapeCoordinate[0][0];
    vertices[0].shapeCoordinate[1] = shapeCoordinate[0][1];
    vertices[0].imageCoordinate[0] = leftCoordinate;
    vertices[0].imageCoordinate[1] = topCoordinate;
    vertices[1].position[0] = radius;
    vertices[1].position[1] = 0.0f;
    vertices[1].shapeCoordinate[0] = shapeCoordinate[1][0];
    vertices[1].shapeCoordinate[1] = shapeCoordinate[1][1];
    vertices[1].imageCoordinate[0] = leftCoordinate + radiusCoordinateWidth;
    vertices[1].imageCoordinate[1] = topCoordinate;
    vertices[2].position[0] = width - radius;
    vertices[2].position[1] = 0.0f;
    vertices[2].shapeCoordinate[0] = shapeCoordinate[2][0];
    vertices[2].shapeCoordinate[1] = shapeCoordinate[2][1];
    vertices[2].imageCoordinate[0] = rightCoordinate - radiusCoordinateWidth;
    vertices[2].imageCoordinate[1] = topCoordinate;
    vertices[3].position[0] = width;
    vertices[3].position[1] = 0.0f;
    vertices[3].shapeCoordinate[0] = shapeCoordinate[3][0];
    vertices[3].shapeCoordinate[1] = shapeCoordinate[3][1];
    vertices[3].imageCoordinate[0] = rightCoordinate;
    vertices[3].imageCoordinate[1] = topCoordinate;

    // Set middle-top row of 4 vertices.
    vertices[4].position[0] = 0.0f;
    vertices[4].position[1] = radius;
    vertices[4].shapeCoordinate[0] = shapeCoordinate[4][0];
    vertices[4].shapeCoordinate[1] = shapeCoordinate[4][1];
    vertices[4].imageCoordinate[0] = leftCoordinate;
    vertices[4].imageCoordinate[1] = topCoordinate + radiusCoordinateHeight;
    vertices[5].position[0] = radius;
    vertices[5].position[1] = radius;
    vertices[5].shapeCoordinate[0] = shapeCoordinate[5][0];
    vertices[5].shapeCoordinate[1] = shapeCoordinate[5][1];
    vertices[5].imageCoordinate[0] = leftCoordinate + radiusCoordinateWidth;
    vertices[5].imageCoordinate[1] = topCoordinate + radiusCoordinateHeight;
    vertices[6].position[0] = width - radius;
    vertices[6].position[1] = radius;
    vertices[6].shapeCoordinate[0] = shapeCoordinate[6][0];
    vertices[6].shapeCoordinate[1] = shapeCoordinate[6][1];
    vertices[6].imageCoordinate[0] = rightCoordinate - radiusCoordinateWidth;
    vertices[6].imageCoordinate[1] = topCoordinate + radiusCoordinateHeight;
    vertices[7].position[0] = width;
    vertices[7].position[1] = radius;
    vertices[7].shapeCoordinate[0] = shapeCoordinate[7][0];
    vertices[7].shapeCoordinate[1] = shapeCoordinate[7][1];
    vertices[7].imageCoordinate[0] = rightCoordinate;
    vertices[7].imageCoordinate[1] = topCoordinate + radiusCoordinateHeight;

    // Set middle-bottom row of 4 vertices.
    vertices[8].position[0] = 0.0f;
    vertices[8].position[1] = height - radius;
    vertices[8].shapeCoordinate[0] = shapeCoordinate[8][0];
    vertices[8].shapeCoordinate[1] = shapeCoordinate[8][1];
    vertices[8].imageCoordinate[0] = leftCoordinate;
    vertices[8].imageCoordinate[1] = bottomCoordinate - radiusCoordinateHeight;
    vertices[9].position[0] = radius;
    vertices[9].position[1] = height - radius;
    vertices[9].shapeCoordinate[0] = shapeCoordinate[9][0];
    vertices[9].shapeCoordinate[1] = shapeCoordinate[9][1];
    vertices[9].imageCoordinate[0] = leftCoordinate + radiusCoordinateWidth;
    vertices[9].imageCoordinate[1] = bottomCoordinate - radiusCoordinateHeight;
    vertices[10].position[0] = width - radius;
    vertices[10].position[1] = height - radius;
    vertices[10].shapeCoordinate[0] = shapeCoordinate[10][0];
    vertices[10].shapeCoordinate[1] = shapeCoordinate[10][1];
    vertices[10].imageCoordinate[0] = rightCoordinate - radiusCoordinateWidth;
    vertices[10].imageCoordinate[1] = bottomCoordinate - radiusCoordinateHeight;
    vertices[11].position[0] = width;
    vertices[11].position[1] = height - radius;
    vertices[11].shapeCoordinate[0] = shapeCoordinate[11][0];
    vertices[11].shapeCoordinate[1] = shapeCoordinate[11][1];
    vertices[11].imageCoordinate[0] = rightCoordinate;
    vertices[11].imageCoordinate[1] = bottomCoordinate - radiusCoordinateHeight;

    // Set bottom row of 4 vertices.
    vertices[12].position[0] = 0.0f;
    vertices[12].position[1] = height;
    vertices[12].shapeCoordinate[0] = shapeCoordinate[12][0];
    vertices[12].shapeCoordinate[1] = shapeCoordinate[12][1];
    vertices[12].imageCoordinate[0] = leftCoordinate;
    vertices[12].imageCoordinate[1] = bottomCoordinate;
    vertices[13].position[0] = radius;
    vertices[13].position[1] = height;
    vertices[13].shapeCoordinate[0] = shapeCoordinate[13][0];
    vertices[13].shapeCoordinate[1] = shapeCoordinate[13][1];
    vertices[13].imageCoordinate[0] = leftCoordinate + radiusCoordinateWidth;
    vertices[13].imageCoordinate[1] = bottomCoordinate;
    vertices[14].position[0] = width - radius;
    vertices[14].position[1] = height;
    vertices[14].shapeCoordinate[0] = shapeCoordinate[14][0];
    vertices[14].shapeCoordinate[1] = shapeCoordinate[14][1];
    vertices[14].imageCoordinate[0] = rightCoordinate - radiusCoordinateWidth;
    vertices[14].imageCoordinate[1] = bottomCoordinate;
    vertices[15].position[0] = width;
    vertices[15].position[1] = height;
    vertices[15].shapeCoordinate[0] = shapeCoordinate[15][0];
    vertices[15].shapeCoordinate[1] = shapeCoordinate[15][1];
    vertices[15].imageCoordinate[0] = rightCoordinate;
    vertices[15].imageCoordinate[1] = bottomCoordinate;

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

// Threshold in grid unit defining the texture quality to be used.
const float lowHighTextureThreshold = 11.0f;

/*!
    \qmltype UbuntuShape
    \instantiates UCUbuntuShape
    \inqmlmodule Ubuntu.Components 1.1
    \ingroup ubuntu
    \brief The UbuntuShape item provides a standard Ubuntu shaped rounded rectangle.

    The UbuntuShape is used where a rounded rectangle is needed either filled
    with a color or an image that it crops.

    When given with a \l color it is applied with an overlay blending as a
    vertical gradient going from \l color to \l gradientColor.
    Two corner \l radius are available, "small" (default) and "medium", that
    determine the size of the corners.
    Optionally, an Image can be passed that will be displayed inside the
    UbuntuShape and cropped to fit it.

    Examples:
    \qml
        import Ubuntu.Components 1.1

        UbuntuShape {
            color: "lightblue"
            radius: "medium"
        }
    \endqml

    \qml
        import Ubuntu.Components 1.1

        UbuntuShape {
            image: Image {
                source: "icon.png"
            }
        }
    \endqml
*/
UCUbuntuShape::UCUbuntuShape(QQuickItem* parent)
    : QQuickItem(parent)
    , imageTextureProvider_(NULL)
    , color_(0.0, 0.0, 0.0, 0.0)
    , colorPremultiplied_(qRgba(0, 0, 0, 0))
    , gradientColor_(0.0, 0.0, 0.0, 0.0)
    , gradientColorPremultiplied_(qRgba(0, 0, 0, 0))
    , gradientColorSet_(false)
    , radiusString_("small")
    , radius_(UCUbuntuShape::SmallRadius)
    , border_(UCUbuntuShape::IdleBorder)
    , image_(NULL)
    , stretched_(true)
    , hAlignment_(UCUbuntuShape::AlignHCenter)
    , vAlignment_(UCUbuntuShape::AlignVCenter)
    , gridUnit_(UCUnits::instance().gridUnit())
    , geometry_()
{
    setFlag(ItemHasContents);
    QObject::connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()), this,
                     SLOT(gridUnitChanged()));
    setImplicitWidth(8 * gridUnit_);
    setImplicitHeight(8 * gridUnit_);
    update();
}

/*!
    \qmlproperty color UbuntuShape::color

    The top color of the gradient used to fill the shape. Setting only this
    one is enough to set the overall color the shape.
*/
void UCUbuntuShape::setColor(const QColor& color)
{
    if (color_ != color) {
        color_ = color;
        const int alpha = color.alpha();
        const QRgb colorPremultiplied = qRgba(
            (color.red() * alpha) / 255,
            (color.green() * alpha) / 255,
            (color.blue() * alpha) / 255,
            alpha);
        colorPremultiplied_ = colorPremultiplied;
        // gradientColor has the same value as color unless it was manually set
        if (!gradientColorSet_) {
            gradientColor_ = color;
            gradientColorPremultiplied_ = colorPremultiplied;
            Q_EMIT gradientColorChanged();
        }
        update();
        Q_EMIT colorChanged();
    }
}

/*!
    \qmlproperty color UbuntuShape::gradientColor

    The bottom color of the gradient used for the overlay blending of the
    color that fills the shape. It is optional to set this one as setting
    \l color is enough to set the overall color of the shape.
*/
void UCUbuntuShape::setGradientColor(const QColor& gradientColor)
{
    gradientColorSet_ = true;
    if (gradientColor_ != gradientColor) {
        gradientColor_ = gradientColor;
        const int alpha = gradientColor.alpha();
        gradientColorPremultiplied_ = qRgba(
            (gradientColor.red() * alpha) / 255,
            (gradientColor.green() * alpha) / 255,
            (gradientColor.blue() * alpha) / 255,
            alpha);
        update();
        Q_EMIT gradientColorChanged();
    }
}

/*!
    \qmlproperty string UbuntuShape::radius

    The size of the corners among: "small" (default) and "medium".
*/
void UCUbuntuShape::setRadius(const QString& radius)
{
    if (radiusString_ != radius) {
        radiusString_ = radius;
        radius_ = (radius == "medium") ? UCUbuntuShape::MediumRadius : UCUbuntuShape::SmallRadius;
        update();
        Q_EMIT radiusChanged();
    }
}

/*!
    \deprecated
    \qmlproperty url UbuntuShape::borderSource

    The image used as a border.
    We plan to expose that feature through styling properties.
*/
void UCUbuntuShape::setBorderSource(const QString& borderSource)
{
    if (borderSource_ != borderSource) {
        if (borderSource.endsWith(QString("radius_idle.sci")))
            border_ = UCUbuntuShape::IdleBorder;
        else if (borderSource.endsWith(QString("radius_pressed.sci")))
            border_ = UCUbuntuShape::PressedBorder;
        else
            border_ = UCUbuntuShape::RawBorder;
        borderSource_ = borderSource;
        update();
        Q_EMIT borderSourceChanged();
    }
}

/*!
    \qmlproperty Image UbuntuShape::image

    The image used to fill the shape.
*/
void UCUbuntuShape::setImage(const QVariant& image)
{
    QQuickItem* newImage = qobject_cast<QQuickItem*>(qvariant_cast<QObject*>(image));
    if (image_ != newImage) {
        image_ = newImage;

        // update values of properties that depend on properties of the image
        QObject::disconnect(image_);
        if (newImage != NULL) {
            updateFromImageProperties(newImage);
            connectToImageProperties(newImage);
        }

        if (image_ && !image_->parentItem()) {
            // Inlined images need a parent and must not be visible.
            image_->setParentItem(this);
            image_->setVisible(false);
        }
        update();
        Q_EMIT imageChanged();
    }
}

void UCUbuntuShape::updateFromImageProperties(QQuickItem* image)
{
    int alignment;

    // UCUbuntuShape::stretched depends on image::fillMode
    QQuickImage::FillMode fillMode = (QQuickImage::FillMode)image->property("fillMode").toInt();
    if (fillMode == QQuickImage::PreserveAspectCrop) {
        setStretched(false);
    } else {
        setStretched(true);
    }

    // UCUbuntuShape::horizontalAlignment depends on image::horizontalAlignment
    int imageHorizontalAlignment = image->property("horizontalAlignment").toInt();
    if (imageHorizontalAlignment == Qt::AlignLeft) {
        alignment = UCUbuntuShape::AlignLeft;
    } else if (imageHorizontalAlignment == Qt::AlignRight) {
        alignment = UCUbuntuShape::AlignRight;
    } else {
        alignment = UCUbuntuShape::AlignHCenter;
    }
    setHorizontalAlignment(static_cast<UCUbuntuShape::HAlignment>(alignment));

    // UCUbuntuShape::verticalAlignment depends on image::verticalAlignment
    int imageVerticalAlignment = image->property("verticalAlignment").toInt();
    if (imageVerticalAlignment == Qt::AlignTop) {
        alignment = UCUbuntuShape::AlignTop;
    } else if (imageVerticalAlignment == Qt::AlignBottom) {
        alignment = UCUbuntuShape::AlignBottom;
    } else {
        alignment = UCUbuntuShape::AlignVCenter;
    }
    setVerticalAlignment(static_cast<UCUbuntuShape::VAlignment>(alignment));
}

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

void UCUbuntuShape::connectToImageProperties(QQuickItem* image)
{
    connectToPropertyChange(image, "fillMode", this, "onImagePropertiesChanged()");
    connectToPropertyChange(image, "horizontalAlignment", this, "onImagePropertiesChanged()");
    connectToPropertyChange(image, "verticalAlignment", this, "onImagePropertiesChanged()");
}

void UCUbuntuShape::onImagePropertiesChanged()
{
    QQuickItem* image = qobject_cast<QQuickItem*>(sender());
    updateFromImageProperties(image);
}

void UCUbuntuShape::setStretched(bool stretched)
{
    if (stretched_ != stretched) {
        stretched_ = stretched;
        update();
        Q_EMIT stretchedChanged();
    }
}

void UCUbuntuShape::setHorizontalAlignment(HAlignment hAlignment)
{
    if (hAlignment_ != hAlignment) {
        hAlignment_ = hAlignment;
        update();
        Q_EMIT horizontalAlignmentChanged();
    }
}

void UCUbuntuShape::setVerticalAlignment(VAlignment vAlignment)
{
    if (vAlignment_ != vAlignment) {
        vAlignment_ = vAlignment;
        update();
        Q_EMIT verticalAlignmentChanged();
    }
}

void UCUbuntuShape::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    geometry_ = newGeometry;
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    update();
}

void UCUbuntuShape::onOpenglContextDestroyed()
{
    QOpenGLContext* context = qobject_cast<QOpenGLContext*>(sender());
    if (Q_UNLIKELY(!context)) {
        return;
    }

    QHash<QOpenGLContext*, ShapeTextures>::iterator it = shapeTexturesHash.find(context);
    if (it != shapeTexturesHash.end()) {
        ShapeTextures &shapeTextures = it.value();
        delete shapeTextures.high;
        delete shapeTextures.low;
        shapeTexturesHash.erase(it);
    }
}

void UCUbuntuShape::gridUnitChanged()
{
    gridUnit_ = UCUnits::instance().gridUnit();
    setImplicitWidth(8 * gridUnit_);
    setImplicitHeight(8 * gridUnit_);
    update();
}

void UCUbuntuShape::providerDestroyed(QObject* object)
{
    Q_UNUSED(object);
    imageTextureProvider_ = NULL;
}

QSGNode* UCUbuntuShape::updatePaintNode(QSGNode* old_node, UpdatePaintNodeData* data)
{
    Q_UNUSED(data);

    // OpenGL allocates textures per context, so we store textures reused by all shape instances
    // per context as well.
    QOpenGLContext* openglContext = window() ? window()->openglContext() : NULL;
    if (Q_UNLIKELY(!openglContext)) {
        qCritical() << "Window has no OpenGL context!";
        delete old_node;
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
                         this, SLOT(onOpenglContextDestroyed()), Qt::DirectConnection);
    }

    ShapeNode* node = static_cast<ShapeNode*>(old_node);
    if (!node) {
        node = new ShapeNode(this);
    }
    ShapeMaterial::Data* materialData = node->material()->data();

    // Update the item whenever the source item's texture changes.
    QSGTextureProvider* provider = image_ ? image_->textureProvider() : NULL;
    if (provider != imageTextureProvider_) {
        if (imageTextureProvider_) {
            QObject::disconnect(imageTextureProvider_, SIGNAL(textureChanged()),
                                this, SLOT(update()));
            QObject::disconnect(imageTextureProvider_, SIGNAL(destroyed()),
                                this, SLOT(providerDestroyed()));
        }
        if (provider) {
            QObject::connect(provider, SIGNAL(textureChanged()), this, SLOT(update()));
            QObject::connect(provider, SIGNAL(destroyed()), this, SLOT(providerDestroyed()));
        }
        imageTextureProvider_ = provider;
    }

    TextureData* textureData;
    if (gridUnit_ > lowHighTextureThreshold) {
        textureData = &shapeTextureHigh;
        materialData->shapeTexture = shapeTextures.high;
    } else {
        textureData = &shapeTextureLow;
        materialData->shapeTexture = shapeTextures.low;
    }

    // Set the shape texture to be used by the materials depending on current grid unit. The radius
    // is set considering the current grid unit and the texture raster grid unit. When the item size
    // is less than 2 radii, the radius is scaled down.
    float radius = (radius_ == UCUbuntuShape::SmallRadius) ?
        textureData->smallRadius : textureData->mediumRadius;
    const float scaleFactor = gridUnit_ / textureData->gridUnit;
    materialData->shapeTextureFiltering = QSGTexture::Nearest;
    if (scaleFactor != 1.0f) {
        radius *= scaleFactor;
        materialData->shapeTextureFiltering = QSGTexture::Linear;
    }
    const float halfMinWidthHeight = qMin(geometry_.width(), geometry_.height()) * 0.5f;
    if (radius > halfMinWidthHeight) {
        radius = halfMinWidthHeight;
        materialData->shapeTextureFiltering = QSGTexture::Linear;
    }

    // Set remaining material data. Colors have to be set to 0 in case of a colored shape and
    // provider has to be set to NULL in case of a textured shape for ShapeMaterial::compare() to
    // behave correctly.
    if (provider && provider->texture()) {
        materialData->imageTextureProvider = imageTextureProvider_;
        materialData->color = qRgba(0, 0, 0, 0);
        materialData->gradientColor = qRgba(0, 0, 0, 0);
        materialData->flags &= ~ShapeMaterial::Data::ColoredFlag;
    } else {
        materialData->imageTextureProvider = NULL;
        materialData->color = colorPremultiplied_;
        materialData->gradientColor = gradientColorPremultiplied_;
        materialData->flags |= ShapeMaterial::Data::ColoredFlag;
    }

    // Update vertices and material.
    int index = (border_ == UCUbuntuShape::RawBorder) ?
        0 : (border_ == UCUbuntuShape::IdleBorder) ? 1 : 2;
    if (radius_ == UCUbuntuShape::SmallRadius)
        index += 3;
    node->setVertices(geometry_, radius, image_, stretched_, hAlignment_, vAlignment_,
                      textureData->coordinate[index]);

    return node;
}
