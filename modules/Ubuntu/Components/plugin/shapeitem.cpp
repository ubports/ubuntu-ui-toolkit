/*
 * Copyright 2013 Canonical Ltd.
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

#include "shapeitem.h"
#include "shapeitemtexture.h"
#include "ucunits.h"
#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGTextureProvider>

// Retrieves the size of an array at compile time.
#define ARRAY_SIZE(a) \
    ((sizeof(a) / sizeof(*(a))) / static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))

// Threshold in grid unit defining the texture quality to be used.
const float lowHighTextureThreshold = 11.0f;

static const char* const shapeVertexShader =
    "uniform lowp mat4 matrix;                  \n"
    "attribute lowp vec4 positionAttrib;        \n"
    "attribute lowp vec2 shapeCoordAttrib;      \n"
    "attribute lowp vec2 imageCoordAttrib;      \n"
    "varying lowp vec2 shapeCoord;              \n"
    "varying lowp vec2 imageCoord;              \n"
    "void main()                                \n"
    "{                                          \n"
    "    shapeCoord = shapeCoordAttrib;         \n"
    "    imageCoord = imageCoordAttrib;         \n"
    "    gl_Position = matrix * positionAttrib; \n"
    "}";

static const char* const shapeTexturedFragmentShader =
    "uniform lowp float opacity;                                                    \n"
    "uniform sampler2D shapeTexture;                                                \n"
    "uniform sampler2D imageTexture;                                                \n"
    "varying lowp vec2 shapeCoord;                                                  \n"
    "varying lowp vec2 imageCoord;                                                  \n"
    "void main()                                                                    \n"
    "{                                                                              \n"
    "    lowp vec4 shapeData = texture2D(shapeTexture, shapeCoord);                 \n"
    "    lowp vec4 color = texture2D(imageTexture, imageCoord) * vec4(shapeData.b); \n"
    "    lowp vec4 blend = shapeData.gggr + vec4(1.0 - shapeData.r) * color;        \n"
    "    gl_FragColor = blend * vec4(opacity);                                      \n"
    "}";

static const char* const shapeColoredFragmentShader =
    "uniform lowp float opacity;                                                                \n"
    "uniform sampler2D shapeTexture;                                                            \n"
    "uniform lowp vec4 baseColor;                                                               \n"
    "uniform lowp vec4 gradientColor;                                                           \n"
    "varying lowp vec2 shapeCoord;                                                              \n"
    "varying lowp vec2 imageCoord;                                                              \n"
    "lowp vec3 blendOverlay(lowp vec3 base, lowp vec3 blend)                                    \n"
    "{                                                                                          \n"
    "    lowp vec3 comparison = clamp(sign(base.rgb - vec3(0.5)), vec3(0.0), vec3(1.0));        \n"
    "    return mix(2.0 * base * blend, 1.0 - 2.0 * (1.0 - base) * (1.0 - blend), comparison);  \n"
    "}                                                                                          \n"
    "void main(void)                                                                            \n"
    "{                                                                                          \n"
    "    lowp vec4 shapeData = texture2D(shapeTexture, shapeCoord);                             \n"
    "    lowp vec4 gradient = gradientColor * imageCoord.t;                                     \n"
    "    lowp vec4 result = vec4(blendOverlay(baseColor.rgb / max(1.0/256.0, baseColor.a),      \n"
    "                                         gradient.rgb / max(1.0/256.0, gradient.a)), 1.0); \n"
    "    result *= baseColor.a;                                                                 \n"
    "    lowp vec4 color = mix(baseColor, result, gradient.a) * vec4(shapeData.b);              \n"
    "    lowp vec4 blend = shapeData.gggr + vec4(1.0 - shapeData.r) * color;                    \n"
    "    gl_FragColor = blend * vec4(opacity);                                                  \n"
    "}";

static const unsigned short shapeMeshIndices[] __attribute__((aligned(16))) = {
    0, 4, 1, 5, 2, 6, 3, 7,       // Triangles 1 to 6.
    7, 4,                         // Degenerate triangles.
    4, 8, 5, 9, 6, 10, 7, 11,     // Triangles 7 to 12.
    11, 8,                        // Degenerate triangles.
    8, 12, 9, 13, 10, 14, 11, 15  // Triangles 13 to 18
};

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

// --- QtQuick item ---

ShapeItem::ShapeItem(QQuickItem* parent)
    : QQuickItem(parent)
    , baseColor_(0.0, 0.0, 0.0, 0.0)
    , gradientColor_(0.0, 0.0, 0.0, 0.0)
    , radiusString_("small")
    , radius_(ShapeItem::SmallRadius)
    , borderSource_("")
    , border_(ShapeItem::RawBorder)
    , image_(NULL)
    , stretched_(true)
    , hAlignment_(ShapeItem::AlignHCenter)
    , vAlignment_(ShapeItem::AlignVCenter)
    , gridUnit_(UCUnits::instance().gridUnit())
    , geometry_()
    , dirtyFlags_(ShapeItem::DirtyAll)
{
    QObject::connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()), this,
                     SLOT(gridUnitChanged()));
    setFlag(ItemHasContents);
    update();
}

void ShapeItem::setBaseColor(const QColor& baseColor)
{
    if (baseColor_ != baseColor) {
        baseColor_ = baseColor;
        dirtyFlags_ |= ShapeItem::DirtyBaseColor;
        update();
        Q_EMIT baseColorChanged();
    }
}

void ShapeItem::setGradientColor(const QColor& gradientColor)
{
    if (gradientColor_ != gradientColor) {
        gradientColor_ = gradientColor;
        dirtyFlags_ |= ShapeItem::DirtyGradientColor;
        update();
        Q_EMIT gradientColorChanged();
    }
}

void ShapeItem::setRadius(const QString& radius)
{
    if (radiusString_ != radius) {
        radiusString_ = radius;
        radius_ = (radius == "medium") ? ShapeItem::MediumRadius : ShapeItem::SmallRadius;
        dirtyFlags_ |= ShapeItem::DirtyRadius;
        update();
        Q_EMIT radiusChanged();
    }
}

void ShapeItem::setBorderSource(const QString& borderSource)
{
    if (borderSource_ != borderSource) {
        if (borderSource.endsWith(QString("radius_idle.sci")))
            border_ = ShapeItem::IdleBorder;
        else if (borderSource.endsWith(QString("radius_pressed.sci")))
            border_ = ShapeItem::PressedBorder;
        else
            border_ = ShapeItem::RawBorder;
        borderSource_ = borderSource;
        dirtyFlags_ |= ShapeItem::DirtyBorder;
        update();
        Q_EMIT borderSourceChanged();
    }
}

void ShapeItem::setImage(QVariant image)
{
    QQuickItem* newImage = qobject_cast<QQuickItem*>(qvariant_cast<QObject*>(image));
    if (image_ != newImage) {
        image_ = newImage;
        if (image_ && !image_->parentItem()) {
            // Inlined images need a parent and must not be visible.
            image_->setParentItem(this);
            image_->setVisible(false);
        }
        dirtyFlags_ |= ShapeItem::DirtyImage;
        update();
        Q_EMIT imageChanged();
    }
}

void ShapeItem::setStretched(bool stretched)
{
    if (stretched_ != stretched) {
        stretched_ = stretched;
        dirtyFlags_ |= ShapeItem::DirtyStretched;
        update();
        Q_EMIT stretchedChanged();
    }
}

void ShapeItem::setHorizontalAlignment(HAlignment hAlignment)
{
    if (hAlignment_ != hAlignment) {
        hAlignment_ = hAlignment;
        dirtyFlags_ |= ShapeItem::DirtyHAlignment;
        update();
        Q_EMIT horizontalAlignmentChanged();
    }
}

void ShapeItem::setVerticalAlignment(VAlignment vAlignment)
{
    if (vAlignment_ != vAlignment) {
        vAlignment_ = vAlignment;
        dirtyFlags_ |= ShapeItem::DirtyVAlignment;
        update();
        Q_EMIT verticalAlignmentChanged();
    }
}

void ShapeItem::gridUnitChanged()
{
    gridUnit_ = UCUnits::instance().gridUnit();
    dirtyFlags_ |= ShapeItem::DirtyGridUnit;
    update();
}

void ShapeItem::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    geometry_ = newGeometry;
    dirtyFlags_ |= ShapeItem::DirtyGeometry;
    update();
}

QSGNode* ShapeItem::updatePaintNode(QSGNode* old_node, UpdatePaintNodeData* data)
{
    Q_UNUSED(data);
    bool setPosition = false;

    // FIXME(loicm) Shape textures are stored in the read-only data section of the plugin as it
    //     avoids having to deal with paths for now. It should preferably be loaded from a file.
    static bool once = false;
    if (!once) {
        shapeTextureHigh.texture = window()->createTextureFromImage(
            QImage(shapeTextureHigh.data, shapeTextureHigh.width, shapeTextureHigh.height,
                   QImage::Format_ARGB32_Premultiplied));
        shapeTextureLow.texture = window()->createTextureFromImage(
            QImage(shapeTextureLow.data, shapeTextureLow.width, shapeTextureLow.height,
                   QImage::Format_ARGB32_Premultiplied));
        once = true;
    }

    ShapeNode* node = static_cast<ShapeNode*>(old_node);
    if (!node) {
        node = new ShapeNode(this);
    }

    // Update dirty parameters.
    ShapeTexturedMaterial* texturedMaterial = node->texturedMaterial();
    ShapeColoredMaterial* coloredMaterial = node->coloredMaterial();

    if (dirtyFlags_ & ShapeItem::DirtyBaseColor)
        coloredMaterial->setBaseColor(baseColor_);
    if (dirtyFlags_ & ShapeItem::DirtyGradientColor)
        coloredMaterial->setGradientColor(gradientColor_);
    if (dirtyFlags_ & (ShapeItem::DirtyBorder | ShapeItem::DirtyRadius
                       | ShapeItem::DirtyGridUnit)) {
        TextureData* textureData = (gridUnit_ > lowHighTextureThreshold) ?
            &shapeTextureHigh : &shapeTextureLow;
        node->setShapeCoordinate(border_, radius_, textureData);
    }
    if (dirtyFlags_ & (ShapeItem::DirtyGeometry | ShapeItem::DirtyRadius
                       | ShapeItem::DirtyStretched | ShapeItem::DirtyHAlignment
                       | ShapeItem::DirtyVAlignment | ShapeItem::DirtyGridUnit)) {
        setPosition = true;
    }
    if (dirtyFlags_ & ShapeItem::DirtyImage) {
        texturedMaterial->setImage(image_);
        node->setMaterialType(image_ ? ShapeNode::TexturedMaterial : ShapeNode::ColoredMaterial);
        setPosition = true;
    }
    dirtyFlags_ = ShapeItem::NotDirty;

    if (setPosition) {
        int scaledDown = 0;
        TextureData* textureData = (gridUnit_ > lowHighTextureThreshold) ?
            &shapeTextureHigh : &shapeTextureLow;
        // Get the radius considering the current grid unit and the texture raster grid unit.
        float radius = (radius_ == ShapeItem::SmallRadius) ?
            textureData->smallRadius : textureData->mediumRadius;
        const float scaleFactor = gridUnit_ / textureData->gridUnit;
        radius *= scaleFactor;
        if (scaleFactor != 1.0f) {
            scaledDown |= 1;
        }
        // When the item size is less than 2 radii, the radius is scaled down anyhow.
        const float halfMinWidthHeight = qMin(geometry_.width(), geometry_.height()) * 0.5f;
        if (radius > halfMinWidthHeight) {
            radius = halfMinWidthHeight;
            scaledDown |= 1;
        }
        // Set the shape texture to be used depending on current grid unit.
        coloredMaterial->setShapeTexture(textureData->texture, !!scaledDown);
        texturedMaterial->setShapeTexture(textureData->texture, !!scaledDown);
        // Update vertex position and shape coordinate attributes.
        node->setPosition(geometry_, radius, image_, stretched_, hAlignment_, vAlignment_);
    }

    return node;
}

// --- Scene graph geometry node ---

ShapeNode::ShapeNode(ShapeItem* item)
    : QSGGeometryNode()
    , item_(item)
    , geometry_(getAttributes(), shapeMesh.vertexCount, shapeMesh.indexCount, shapeMesh.indexType)
    , texturedMaterial_()
    , coloredMaterial_()
    , currentMaterial_(ShapeNode::ColoredMaterial)
{
    memcpy(geometry_.indexData(), shapeMesh.indices,
           shapeMesh.indexCount * sizeOfType(shapeMesh.indexType));
    geometry_.setDrawingMode(GL_TRIANGLE_STRIP);
    setGeometry(&geometry_);
    setMaterial(&coloredMaterial_);
    setFlag(UsePreprocess, false);
    markDirty(DirtyGeometry);
    markDirty(DirtyMaterial);
}

void ShapeNode::setPosition(const QRectF& geometry, float radius, QQuickItem* image, bool stretched,
                            ShapeItem::HAlignment hAlignment, ShapeItem::VAlignment vAlignment)
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
            radiusCoordinateHeight = (radius - 1.0f) / (height - 1.0f);
            radiusCoordinateWidth = ((radius - 1.0f) / (width - 1.0f)) * inCoordinateSize;
        } else {
            const float inCoordinateSize = srcRatio / dstRatio;
            const float outCoordinateSize = 1.0f - inCoordinateSize;
            topCoordinate = outCoordinateSize * factors[vAlignment];
            bottomCoordinate = 1.0f - (outCoordinateSize * (1.0f - factors[vAlignment]));
            leftCoordinate = 0.0f;
            rightCoordinate = 1.0f;
            radiusCoordinateHeight = ((radius - 1.0f) / (height - 1.0f)) * inCoordinateSize;
            radiusCoordinateWidth = (radius - 1.0f) / (width - 1.0f);
        }
    } else {
        // Don't preserve source image aspect ratio stretching it in destination rectangle.
        topCoordinate = 0.0f;
        bottomCoordinate = 1.0f;
        leftCoordinate = 0.0f;
        rightCoordinate = 1.0f;
        radiusCoordinateHeight = (radius - 1.0f) / (height - 1.0f);
        radiusCoordinateWidth = (radius - 1.0f) / (width - 1.0f);
    }

    // Set top row of 4 vertices.
    vertices[0].position[0] = 0.0f;
    vertices[0].position[1] = 0.0f;
    vertices[0].imageCoordinate[0] = leftCoordinate;
    vertices[0].imageCoordinate[1] = topCoordinate;
    vertices[1].position[0] = radius;
    vertices[1].position[1] = 0.0f;
    vertices[1].imageCoordinate[0] = radiusCoordinateWidth;
    vertices[1].imageCoordinate[1] = topCoordinate;
    vertices[2].position[0] = width - radius;
    vertices[2].position[1] = 0.0f;
    vertices[2].imageCoordinate[0] = rightCoordinate - radiusCoordinateWidth;
    vertices[2].imageCoordinate[1] = topCoordinate;
    vertices[3].position[0] = width;
    vertices[3].position[1] = 0.0f;
    vertices[3].imageCoordinate[0] = rightCoordinate;
    vertices[3].imageCoordinate[1] = topCoordinate;

    // Set middle-top row of 4 vertices.
    vertices[4].position[0] = 0.0f;
    vertices[4].position[1] = radius;
    vertices[4].imageCoordinate[0] = leftCoordinate;
    vertices[4].imageCoordinate[1] = topCoordinate + radiusCoordinateHeight;
    vertices[5].position[0] = radius;
    vertices[5].position[1] = radius;
    vertices[5].imageCoordinate[0] = leftCoordinate + radiusCoordinateWidth;
    vertices[5].imageCoordinate[1] = topCoordinate + radiusCoordinateHeight;
    vertices[6].position[0] = width - radius;
    vertices[6].position[1] = radius;
    vertices[6].imageCoordinate[0] = rightCoordinate - radiusCoordinateWidth;
    vertices[6].imageCoordinate[1] = topCoordinate + radiusCoordinateHeight;
    vertices[7].position[0] = width;
    vertices[7].position[1] = radius;
    vertices[7].imageCoordinate[0] = rightCoordinate;
    vertices[7].imageCoordinate[1] = topCoordinate + radiusCoordinateHeight;

    // Set middle-bottom row of 4 vertices.
    vertices[8].position[0] = 0.0f;
    vertices[8].position[1] = height - radius;
    vertices[8].imageCoordinate[0] = leftCoordinate;
    vertices[8].imageCoordinate[1] = bottomCoordinate - radiusCoordinateHeight;
    vertices[9].position[0] = radius;
    vertices[9].position[1] = height - radius;
    vertices[9].imageCoordinate[0] = leftCoordinate + radiusCoordinateWidth;
    vertices[9].imageCoordinate[1] = bottomCoordinate - radiusCoordinateHeight;
    vertices[10].position[0] = width - radius;
    vertices[10].position[1] = height - radius;
    vertices[10].imageCoordinate[0] = rightCoordinate - radiusCoordinateWidth;
    vertices[10].imageCoordinate[1] = bottomCoordinate - radiusCoordinateHeight;
    vertices[11].position[0] = width;
    vertices[11].position[1] = height - radius;
    vertices[11].imageCoordinate[0] = rightCoordinate;
    vertices[11].imageCoordinate[1] = bottomCoordinate - radiusCoordinateHeight;

    // Set bottom row of 4 vertices.
    vertices[12].position[0] = 0.0f;
    vertices[12].position[1] = height;
    vertices[12].imageCoordinate[0] = leftCoordinate;
    vertices[12].imageCoordinate[1] = bottomCoordinate;
    vertices[13].position[0] = radius;
    vertices[13].position[1] = height;
    vertices[13].imageCoordinate[0] = leftCoordinate + radiusCoordinateWidth;
    vertices[13].imageCoordinate[1] = bottomCoordinate;
    vertices[14].position[0] = width - radius;
    vertices[14].position[1] = height;
    vertices[14].imageCoordinate[0] = rightCoordinate - radiusCoordinateWidth;
    vertices[14].imageCoordinate[1] = bottomCoordinate;
    vertices[15].position[0] = width;
    vertices[15].position[1] = height;
    vertices[15].imageCoordinate[0] = rightCoordinate;
    vertices[15].imageCoordinate[1] = bottomCoordinate;

    markDirty(DirtyGeometry);
}

void ShapeNode::setShapeCoordinate(ShapeItem::Border border, ShapeItem::Radius radius,
                                   TextureData* textureData)
{
    ShapeNode::Vertex* vertices = reinterpret_cast<ShapeNode::Vertex*>(geometry_.vertexData());
    int index = (border == ShapeItem::RawBorder) ? 0 : (border == ShapeItem::IdleBorder) ? 1 : 2;
    if (radius == ShapeItem::SmallRadius)
        index += 3;
    for (int i = 0; i < 16; i++) {
        vertices[i].shapeCoordinate[0] = textureData->coordinate[index][i][0];
        vertices[i].shapeCoordinate[1] = textureData->coordinate[index][i][1];
    }
    markDirty(DirtyGeometry);
}

void ShapeNode::setMaterialType(ShapeNode::MaterialType material)
{
    if (currentMaterial_ != material) {
        if (material == ShapeNode::ColoredMaterial)
            setMaterial(&coloredMaterial_);
        else
            setMaterial(&texturedMaterial_);
        markDirty(DirtyMaterial);
    }
}

// --- Scene graph textured material ---

ShapeTexturedMaterial::ShapeTexturedMaterial()
    : imageTextureProvider_(NULL)
{
    setFlag(Blending);
}

QSGMaterialType* ShapeTexturedMaterial::type() const
{
    static QSGMaterialType type;
    return &type;
}

QSGMaterialShader* ShapeTexturedMaterial::createShader() const
{
    return new ShapeTexturedShader;
}

void ShapeTexturedMaterial::setImage(QQuickItem* image)
{
    imageTextureProvider_ = image ? image->textureProvider() : NULL;
}

QSGTextureProvider* ShapeTexturedMaterial::imageTextureProvider() const
{
    return imageTextureProvider_;
}

void ShapeTexturedMaterial::setShapeTexture(QSGTexture* texture, bool scaledDown)
{
    shapeTexture_ = texture;
    filtering_ = scaledDown ? QSGTexture::Linear : QSGTexture::Nearest;
}

// -- Scene graph textured material shader ---

const char *ShapeTexturedShader::vertexShader() const
{
    return shapeVertexShader;
}

const char* ShapeTexturedShader::fragmentShader() const
{
    return shapeTexturedFragmentShader;
}

char const* const* ShapeTexturedShader::attributeNames() const
{
    static char const* const attributes[] = {
        "positionAttrib", "shapeCoordAttrib", "imageCoordAttrib", 0
    };
    return attributes;
}

void ShapeTexturedShader::initialize()
{
    QSGMaterialShader::initialize();
    program()->bind();
    program()->setUniformValue("shapeTexture", 0);
    program()->setUniformValue("imageTexture", 1);
    matrixId_ = program()->uniformLocation("matrix");
    opacityId_ = program()->uniformLocation("opacity");
    glFuncs_ = QOpenGLContext::currentContext()->functions();
}

void ShapeTexturedShader::updateState(const RenderState& state, QSGMaterial* newEffect,
                                      QSGMaterial* oldEffect)
{
    Q_UNUSED(oldEffect);
    ShapeTexturedMaterial* material = static_cast<ShapeTexturedMaterial*>(newEffect);

    // Bind textures.
    glFuncs_->glActiveTexture(GL_TEXTURE1);
    QSGTextureProvider* provider = material->imageTextureProvider();
    QSGTexture* texture = provider ? provider->texture() : NULL;
    if (texture)
        texture->bind();
    else
        glBindTexture(GL_TEXTURE_2D, 0);
    QSGTexture* shapeTexture = material->shapeTexture();
    glFuncs_->glActiveTexture(GL_TEXTURE0);
    shapeTexture->setFiltering(material->filtering());
    shapeTexture->setHorizontalWrapMode(QSGTexture::ClampToEdge);
    shapeTexture->setVerticalWrapMode(QSGTexture::ClampToEdge);
    shapeTexture->bind();

    // Bind uniforms.
    if (state.isMatrixDirty())
        program()->setUniformValue(matrixId_, state.combinedMatrix());
    if (state.isOpacityDirty())
        program()->setUniformValue(opacityId_, state.opacity());
}

// --- Scene graph colored material ---

ShapeColoredMaterial::ShapeColoredMaterial()
    : baseColor_(0.0, 0.0, 0.0, 0.0)
    , gradientColor_(0.0, 0.0, 0.0, 0.0)
{
    setFlag(Blending);
}

QSGMaterialType* ShapeColoredMaterial::type() const
{
    static QSGMaterialType type;
    return &type;
}

QSGMaterialShader* ShapeColoredMaterial::createShader() const
{
    return new ShapeColoredShader;
}

void ShapeColoredMaterial::setBaseColor(const QColor& baseColor)
{
    // Premultiply color components by alpha.
    const float alpha = baseColor.alphaF();
    baseColor_ = QVector4D(baseColor.redF() * alpha, baseColor.greenF() * alpha,
                           baseColor.blueF() * alpha, alpha);
}

void ShapeColoredMaterial::setGradientColor(const QColor& gradientColor)
{
    // Premultiply color components by alpha.
    const float alpha = gradientColor.alphaF();
    gradientColor_ = QVector4D(gradientColor.redF() * alpha, gradientColor.greenF() * alpha,
                               gradientColor.blueF() * alpha, alpha);
}

void ShapeColoredMaterial::setShapeTexture(QSGTexture* texture, bool scaledDown)
{
    shapeTexture_ = texture;
    filtering_ = scaledDown ? QSGTexture::Linear : QSGTexture::Nearest;
}

// -- Scene graph colored material shader ---

const char *ShapeColoredShader::vertexShader() const
{
    return shapeVertexShader;
}

const char* ShapeColoredShader::fragmentShader() const
{
    return shapeColoredFragmentShader;
}

char const* const* ShapeColoredShader::attributeNames() const
{
    static char const* const attributes[] = {
        "positionAttrib", "shapeCoordAttrib", "imageCoordAttrib", 0
    };
    return attributes;
}

void ShapeColoredShader::initialize()
{
    QSGMaterialShader::initialize();
    program()->bind();
    program()->setUniformValue("shapeTexture", 0);
    matrixId_ = program()->uniformLocation("matrix");
    opacityId_ = program()->uniformLocation("opacity");
    baseColorId_ = program()->uniformLocation("baseColor");
    gradientColorId_ = program()->uniformLocation("gradientColor");
}

void ShapeColoredShader::updateState(const RenderState& state, QSGMaterial* newEffect,
                                     QSGMaterial* oldEffect)
{
    Q_UNUSED(oldEffect);
    ShapeColoredMaterial* material = static_cast<ShapeColoredMaterial*>(newEffect);

    // Bind texture.
    QSGTexture* shapeTexture = material->shapeTexture();
    shapeTexture->setFiltering(material->filtering());
    shapeTexture->setHorizontalWrapMode(QSGTexture::ClampToEdge);
    shapeTexture->setVerticalWrapMode(QSGTexture::ClampToEdge);
    shapeTexture->bind();

    // Bind uniforms.
    if (state.isMatrixDirty())
        program()->setUniformValue(matrixId_, state.combinedMatrix());
    if (state.isOpacityDirty())
        program()->setUniformValue(opacityId_, state.opacity());
    program()->setUniformValue(baseColorId_, material->baseColor());
    program()->setUniformValue(gradientColorId_, material->gradientColor());
}
