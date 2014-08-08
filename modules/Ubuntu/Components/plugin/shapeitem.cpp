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
#include <QtCore/QPointer>
#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGTextureProvider>
#include <QtQuick/private/qquickimage_p.h>

/*!
    \qmltype UbuntuShape
    \instantiates ShapeItem
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

/*!
    \qmlproperty string UbuntuShape::radius

    The size of the corners among: "small" (default) and "medium".
*/

/*!
    \qmlproperty color UbuntuShape::color

    The top color of the gradient used to fill the shape. Setting only this
    one is enough to set the overall color the shape.
*/

/*!
    \qmlproperty color UbuntuShape::gradientColor

    The bottom color of the gradient used for the overlay blending of the
    color that fills the shape. It is optional to set this one as setting
    \l color is enough to set the overall color of the shape.
*/

/*!
    \deprecated
    \qmlproperty url UbuntuShape::borderSource

    The image used as a border.
    We plan to expose that feature through styling properties.
*/

/*!
    \qmlproperty Image UbuntuShape::image

    The image used to fill the shape.
*/

// Retrieves the size of an array at compile time.
#define ARRAY_SIZE(a) \
    ((sizeof(a) / sizeof(*(a))) / static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))

// Threshold in grid unit defining the texture quality to be used.
const float lowHighTextureThreshold = 11.0f;

// Map of windows and associated textures.
QHash<QOpenGLContext*, ShapeItem::TextureHandles> ShapeItem::textures_;

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
    "uniform lowp vec4 color;                                                               \n"
    "uniform lowp vec4 gradientColor;                                                           \n"
    "varying lowp vec2 shapeCoord;                                                              \n"
    "varying lowp vec2 imageCoord;                                                              \n"
    "void main(void)                                                                            \n"
    "{                                                                                          \n"
    "    lowp vec4 shapeData = texture2D(shapeTexture, shapeCoord);                             \n"
    "    lowp vec4 result = mix(color, gradientColor, imageCoord.t) * vec4(shapeData.b);        \n"
    "    lowp vec4 blend = shapeData.gggr + vec4(1.0 - shapeData.r) * result;                   \n"
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
    , provider_(NULL)
    , color_(0.0, 0.0, 0.0, 0.0)
    , gradientColor_(0.0, 0.0, 0.0, 0.0)
    , gradientColorSet_(false)
    , radiusString_("small")
    , radius_(ShapeItem::SmallRadius)
    , border_(ShapeItem::IdleBorder)
    , image_(NULL)
    , stretched_(true)
    , hAlignment_(ShapeItem::AlignHCenter)
    , vAlignment_(ShapeItem::AlignVCenter)
    , gridUnit_(UCUnits::instance().gridUnit())
    , geometry_()
    , dirtyFlags_(ShapeItem::DirtyAll)
{
    setFlag(ItemHasContents);
    QObject::connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()), this,
                     SLOT(gridUnitChanged()));
    setImplicitWidth(8 * gridUnit_);
    setImplicitHeight(8 * gridUnit_);
    update();
}

void ShapeItem::setColor(const QColor& color)
{
    if (color_ != color) {
        color_ = color;
        dirtyFlags_ |= ShapeItem::DirtyColor;
        // gradientColor has the same value as color unless it was manually set
        if (!gradientColorSet_) {
            gradientColor_ = color;
            dirtyFlags_ |= ShapeItem::DirtyGradientColor;
            Q_EMIT gradientColorChanged();
        }
        update();
        Q_EMIT colorChanged();
    }
}

void ShapeItem::setGradientColor(const QColor& gradientColor)
{
    gradientColorSet_ = true;
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

void ShapeItem::setImage(const QVariant& image)
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
        dirtyFlags_ |= ShapeItem::DirtyImage;
        update();
        Q_EMIT imageChanged();
    }
}

void ShapeItem::updateFromImageProperties(QQuickItem* image)
{
    int alignment;

    // ShapeItem::stretched depends on image::fillMode
    QQuickImage::FillMode fillMode = (QQuickImage::FillMode)image->property("fillMode").toInt();
    if (fillMode == QQuickImage::PreserveAspectCrop) {
        setStretched(false);
    } else {
        setStretched(true);
    }

    // ShapeItem::horizontalAlignment depends on image::horizontalAlignment
    int imageHorizontalAlignment = image->property("horizontalAlignment").toInt();
    if (imageHorizontalAlignment == Qt::AlignLeft) {
        alignment = ShapeItem::AlignLeft;
    } else if (imageHorizontalAlignment == Qt::AlignRight) {
        alignment = ShapeItem::AlignRight;
    } else {
        alignment = ShapeItem::AlignHCenter;
    }
    setHorizontalAlignment(static_cast<ShapeItem::HAlignment>(alignment));

    // ShapeItem::verticalAlignment depends on image::verticalAlignment
    int imageVerticalAlignment = image->property("verticalAlignment").toInt();
    if (imageVerticalAlignment == Qt::AlignTop) {
        alignment = ShapeItem::AlignTop;
    } else if (imageVerticalAlignment == Qt::AlignBottom) {
        alignment = ShapeItem::AlignBottom;
    } else {
        alignment = ShapeItem::AlignVCenter;
    }
    setVerticalAlignment(static_cast<ShapeItem::VAlignment>(alignment));
}

void ShapeItem::connectToPropertyChange(QObject* sender, const char* property,
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

void ShapeItem::connectToImageProperties(QQuickItem* image)
{
    connectToPropertyChange(image, "fillMode", this, "onImagePropertiesChanged()");
    connectToPropertyChange(image, "horizontalAlignment", this, "onImagePropertiesChanged()");
    connectToPropertyChange(image, "verticalAlignment", this, "onImagePropertiesChanged()");
}

void ShapeItem::onImagePropertiesChanged()
{
    QQuickItem* image = qobject_cast<QQuickItem*>(sender());
    updateFromImageProperties(image);
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
    setImplicitWidth(8 * gridUnit_);
    setImplicitHeight(8 * gridUnit_);
    dirtyFlags_ |= ShapeItem::DirtyGridUnit;
    update();
}

void ShapeItem::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    geometry_ = newGeometry;
    dirtyFlags_ |= ShapeItem::DirtyGeometry;
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    update();
}

void ShapeItem::onOpenglContextDestroyed()
{
    QOpenGLContext* context = qobject_cast<QOpenGLContext*>(sender());
    if (Q_UNLIKELY(!context)) return;

    QHash<QOpenGLContext*, TextureHandles>::iterator it =
        textures_.find(context);
    if (it != textures_.end()) {
        TextureHandles &textureHandles = it.value();
        delete textureHandles.high;
        delete textureHandles.low;
        textures_.erase(it);
    }
}

void ShapeItem::providerDestroyed(QObject* object)
{
    Q_UNUSED(object);
    provider_ = NULL;
}

QSGNode* ShapeItem::updatePaintNode(QSGNode* old_node, UpdatePaintNodeData* data)
{
    Q_UNUSED(data);

    // FIXME(loicm) Shape textures are stored in the read-only data section of the plugin as it
    //     avoids having to deal with paths for now. It should preferably be loaded from a file.

    // OpenGL allocates textures per context, so we store textures reused by
    // all shape instances per context as well
    QOpenGLContext* openglContext = window() ? window()->openglContext() : NULL;
    if (Q_UNLIKELY(!openglContext)) {
        qCritical() << "Window has no GL context!";
        delete old_node;
        return NULL;
    }

    TextureHandles &textureHandles = textures_[openglContext];
    // If the hash table didn't contain an entry for the current context, the
    // line above has just caused the creation of a default-constructed value.
    if (!textureHandles.high) {
        textureHandles.high = window()->createTextureFromImage(
            QImage(shapeTextureHigh.data, shapeTextureHigh.width, shapeTextureHigh.height,
                   QImage::Format_ARGB32_Premultiplied));
        textureHandles.low = window()->createTextureFromImage(
            QImage(shapeTextureLow.data, shapeTextureLow.width, shapeTextureLow.height,
                   QImage::Format_ARGB32_Premultiplied));
        QObject::connect(openglContext, SIGNAL(aboutToBeDestroyed()),
                         this, SLOT(onOpenglContextDestroyed()),
                         Qt::DirectConnection);
    }

    // Update the node whenever the source item's texture changes.
    QSGTextureProvider* provider = image_ ? image_->textureProvider() : NULL;
    if (provider != provider_) {
        if (provider_) {
            QObject::disconnect(provider_, SIGNAL(textureChanged()), this, SLOT(update()));
            QObject::disconnect(provider_, SIGNAL(destroyed()), this, SLOT(providerDestroyed()));
        }
        if (provider) {
            QObject::connect(provider, SIGNAL(textureChanged()), this, SLOT(update()));
            QObject::connect(provider, SIGNAL(destroyed()), this, SLOT(providerDestroyed()));
        }
        provider_ = provider;
    }

    ShapeNode* node = static_cast<ShapeNode*>(old_node);
    if (!node) {
        node = new ShapeNode(this);
    }

    ShapeTexturedMaterial* texturedMaterial = node->texturedMaterial();
    ShapeColoredMaterial* coloredMaterial = node->coloredMaterial();
    TextureData* textureData;
    QSGTexture* textureHandle;
    if (gridUnit_ > lowHighTextureThreshold) {
        textureData = &shapeTextureHigh;
        textureHandle = textureHandles.high;
    } else {
        textureData = &shapeTextureLow;
        textureHandle = textureHandles.low;
    }

    // Set the shape texture to be used by the materials depending on current grid unit. The radius
    // is set considering the current grid unit and the texture raster grid unit. When the item size
    // is less than 2 radii, the radius is scaled down anyhow.
    float radius = (radius_ == ShapeItem::SmallRadius) ?
        textureData->smallRadius : textureData->mediumRadius;
    const float scaleFactor = gridUnit_ / textureData->gridUnit;
    radius *= scaleFactor;
    int scaledDown = 0;
    if (scaleFactor != 1.0f) {
        scaledDown |= 1;
    }
    const float halfMinWidthHeight = qMin(geometry_.width(), geometry_.height()) * 0.5f;
    if (radius > halfMinWidthHeight) {
        radius = halfMinWidthHeight;
        scaledDown |= 1;
    }
    coloredMaterial->setShapeTexture(textureHandle, !!scaledDown);
    texturedMaterial->setShapeTexture(textureHandle, !!scaledDown);

    // Update the other material properties.
    coloredMaterial->setColor(color_);
    coloredMaterial->setGradientColor(gradientColor_);
    texturedMaterial->setImage(image_);

    // Update node vertices and type.
    int index = (border_ == ShapeItem::RawBorder) ?
        0 : (border_ == ShapeItem::IdleBorder) ? 1 : 2;
    if (radius_ == ShapeItem::SmallRadius)
        index += 3;
    node->setVertices(geometry_, radius, image_, stretched_, hAlignment_, vAlignment_,
                      textureData->coordinate[index]);
    const QSGTexture* texture = provider ? provider->texture() : NULL;
    node->setMaterialType(texture ? ShapeNode::TexturedMaterial : ShapeNode::ColoredMaterial);

    dirtyFlags_ = ShapeItem::NotDirty;

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
    geometry_.setIndexDataPattern(QSGGeometry::StaticPattern);
    geometry_.setVertexDataPattern(QSGGeometry::AlwaysUploadPattern);
    setGeometry(&geometry_);
    setMaterial(&coloredMaterial_);
    setFlag(UsePreprocess, false);
}

void ShapeNode::setVertices(const QRectF& geometry, float radius, QQuickItem* image, bool stretched,
                            ShapeItem::HAlignment hAlignment, ShapeItem::VAlignment vAlignment,
                            float shapeCoordinate[][2])
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

void ShapeNode::setMaterialType(ShapeNode::MaterialType material)
{
    if (currentMaterial_ != material) {
        if (material == ShapeNode::ColoredMaterial)
            setMaterial(&coloredMaterial_);
        else
            setMaterial(&texturedMaterial_);
        currentMaterial_ = material;
        markDirty(DirtyMaterial);
    }
}

// --- Scene graph textured material ---

ShapeTexturedMaterial::ShapeTexturedMaterial()
    : imageTextureProvider_(NULL)
    , shapeTexture_(NULL)
    , filtering_(QSGTexture::Nearest)
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

int ShapeTexturedMaterial::compare(const QSGMaterial* other) const
{
    const ShapeTexturedMaterial* otherMaterial = static_cast<const ShapeTexturedMaterial*>(other);
    const QSGTextureProvider* otherTextureProvider = otherMaterial->imageTextureProvider();
    const QSGTexture* otherTexture = otherTextureProvider ? otherTextureProvider->texture() : NULL;
    const int otherTextureId = otherTexture ? otherTexture->textureId() : 0;
    const QSGTexture* texture = imageTextureProvider_ ? imageTextureProvider_->texture() : NULL;
    const int textureId = texture ? texture->textureId() : 0;
    return textureId - otherTextureId;
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
    glFuncs_->glActiveTexture(GL_TEXTURE0);
    QSGTexture* shapeTexture = material->shapeTexture();
    if (shapeTexture) {
        shapeTexture->setFiltering(material->filtering());
        shapeTexture->setHorizontalWrapMode(QSGTexture::ClampToEdge);
        shapeTexture->setVerticalWrapMode(QSGTexture::ClampToEdge);
        shapeTexture->bind();
    } else {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // Bind uniforms.
    if (state.isMatrixDirty())
        program()->setUniformValue(matrixId_, state.combinedMatrix());
    if (state.isOpacityDirty())
        program()->setUniformValue(opacityId_, state.opacity());
}

// --- Scene graph colored material ---

ShapeColoredMaterial::ShapeColoredMaterial()
    : color_(0.0, 0.0, 0.0, 0.0)
    , gradientColor_(0.0, 0.0, 0.0, 0.0)
    , shapeTexture_(NULL)
    , filtering_(QSGTexture::Nearest)
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

int ShapeColoredMaterial::compare(const QSGMaterial* other) const
{
    const ShapeColoredMaterial* otherMaterial = static_cast<const ShapeColoredMaterial*>(other);
    if ((color_ != otherMaterial->color()) || (gradientColor_ != otherMaterial->gradientColor())) {
        return -1;
    } else {
        return 0;
    }
}

void ShapeColoredMaterial::setColor(const QColor& color)
{
    // Premultiply color components by alpha.
    const float alpha = color.alphaF();
    color_ = QVector4D(color.redF() * alpha, color.greenF() * alpha,
                           color.blueF() * alpha, alpha);
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
    colorId_ = program()->uniformLocation("color");
    gradientColorId_ = program()->uniformLocation("gradientColor");
}

void ShapeColoredShader::updateState(const RenderState& state, QSGMaterial* newEffect,
                                     QSGMaterial* oldEffect)
{
    Q_UNUSED(oldEffect);
    ShapeColoredMaterial* material = static_cast<ShapeColoredMaterial*>(newEffect);

    // Bind texture.
    QSGTexture* shapeTexture = material->shapeTexture();
    if (shapeTexture) {
        shapeTexture->setFiltering(material->filtering());
        shapeTexture->setHorizontalWrapMode(QSGTexture::ClampToEdge);
        shapeTexture->setVerticalWrapMode(QSGTexture::ClampToEdge);
        shapeTexture->bind();
    } else {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // Bind uniforms.
    if (state.isMatrixDirty())
        program()->setUniformValue(matrixId_, state.combinedMatrix());
    if (state.isOpacityDirty())
        program()->setUniformValue(opacityId_, state.opacity());
    program()->setUniformValue(colorId_, material->color());
    program()->setUniformValue(gradientColorId_, material->gradientColor());
}
