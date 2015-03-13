/*
 * Copyright 2013-2015 Canonical Ltd.
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

// FIXME(loicm) Storing lower precision data types in the vertex buffer could be more efficent. On
//     PowerVR, for instance, that requires a conversion so the trade-off between shader cycles and
//     bandwidth requirements must be benchmarked.

#include "ucubuntushape.h"
#include "ucubuntushapetexture.h"
#include "ucunits.h"
#include <QtCore/QPointer>
#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGTextureProvider>
#include <QtQuick/private/qquickimage_p.h>
#include <math.h>

// --- Scene graph shader ---

ShapeShader::ShapeShader()
{
    setShaderSourceFile(QOpenGLShader::Vertex, QStringLiteral(":/uc/shaders/shape.vert"));
    setShaderSourceFile(QOpenGLShader::Fragment, QStringLiteral(":/uc/shaders/shape.frag"));
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

    m_functions = QOpenGLContext::currentContext()->functions();
    m_matrixId = program()->uniformLocation("matrix");
    m_opacityId = program()->uniformLocation("opacity");
    m_sourceOpacityId = program()->uniformLocation("sourceOpacity");
    m_texturedId = program()->uniformLocation("textured");
}

void ShapeShader::updateState(
    const RenderState& state, QSGMaterial* newEffect, QSGMaterial* oldEffect)
{
    Q_UNUSED(oldEffect);

    const ShapeMaterial::Data* data = static_cast<ShapeMaterial*>(newEffect)->constData();

    // Bind shape texture.
    QSGTexture* shapeTexture = data->shapeTexture;
    if (shapeTexture) {
        shapeTexture->setFiltering(static_cast<QSGTexture::Filtering>(data->shapeTextureFiltering));
        shapeTexture->setHorizontalWrapMode(QSGTexture::ClampToEdge);
        shapeTexture->setVerticalWrapMode(QSGTexture::ClampToEdge);
        shapeTexture->bind();
    } else {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (data->flags & ShapeMaterial::Data::Textured) {
        // Bind image texture.
        m_functions->glActiveTexture(GL_TEXTURE1);
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
        m_functions->glActiveTexture(GL_TEXTURE0);
        // Update image uniform.
        const float u8toF32 = 1.0f / 255.0f;
        program()->setUniformValue(m_sourceOpacityId, data->sourceOpacity * u8toF32);
    }

    program()->setUniformValue(m_texturedId, !!(data->flags & ShapeMaterial::Data::Textured));

    // Update QtQuick engine uniforms.
    if (state.isMatrixDirty()) {
        program()->setUniformValue(m_matrixId, state.combinedMatrix());
    }
    if (state.isOpacityDirty()) {
        program()->setUniformValue(m_opacityId, state.opacity());
    }
}

// --- Scene graph material ---

ShapeMaterial::ShapeMaterial()
{
    // The whole struct (with the padding bytes) must be initialized for memcmp() to work as
    // expected in ShapeMaterial::compare().
    memset(&m_data, 0x00, sizeof(Data));
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
    // Repeat wrap modes require textures to be extracted from their atlases. Since we just store
    // the texture provider in the material data (not the texture as we want to do the extraction at
    // QSGShader::updateState() time), we make the comparison fail when repeat wrapping is set.
    const ShapeMaterial::Data* otherData = static_cast<const ShapeMaterial*>(other)->constData();
    return memcmp(&m_data, otherData, sizeof(m_data))
        | (m_data.flags & ShapeMaterial::Data::Repeated);
}

// --- Scene graph node ---

ShapeNode::ShapeNode()
    : QSGGeometryNode()
    , m_material()
    , m_geometry(attributeSet(), vertexCount, indexCount, indexType)
{
    memcpy(m_geometry.indexData(), indices(), indexCount * indexTypeSize);
    m_geometry.setDrawingMode(drawingMode);
    m_geometry.setIndexDataPattern(indexDataPattern);
    m_geometry.setVertexDataPattern(vertexDataPattern);
    setMaterial(&m_material);
    setGeometry(&m_geometry);
}

// static
const unsigned short* ShapeNode::indices()
{
    // Don't forget to update indexCount if changed.
    static const unsigned short indices[] = {
        0, 4, 1, 5, 2, 6, 3, 7,       // Triangles 1 to 6.
        7, 4,                         // Degenerate triangles.
        4, 8, 5, 9, 6, 10, 7, 11,     // Triangles 7 to 12.
        11, 8,                        // Degenerate triangles.
        8, 12, 9, 13, 10, 14, 11, 15  // Triangles 13 to 18.
    };
    return indices;
}

// static
const QSGGeometry::AttributeSet& ShapeNode::attributeSet()
{
    static const QSGGeometry::Attribute attributes[] = {
        QSGGeometry::Attribute::create(0, 2, GL_FLOAT, true),
        QSGGeometry::Attribute::create(1, 2, GL_FLOAT),
        QSGGeometry::Attribute::create(2, 4, GL_FLOAT),
        QSGGeometry::Attribute::create(3, 4, GL_UNSIGNED_BYTE)
    };
    static const QSGGeometry::AttributeSet attributeSet = {
        4, sizeof(Vertex), attributes
    };
    return attributeSet;
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
    \inqmlmodule Ubuntu.Components 1.2
    \ingroup ubuntu
    \brief Rounded rectangle containing a source image blended over a background color.

    The UbuntuShape is a rounded rectangle (based on a \l
    {https://en.wikipedia.org/wiki/Squircle}{squircle}) containing an optional source image blended
    over a background color (solid or linear gradient). Different properties allow to change the
    look of the shape.

    Examples:

    \qml
    import Ubuntu.Components 1.2

    UbuntuShape {
        backgroundColor: "green"
    }
    \endqml

    \qml
    import Ubuntu.Components 1.2

    UbuntuShape {
        source: Image {
            source: "ubuntu.png"
        }
    }
    \endqml
*/
UCUbuntuShape::UCUbuntuShape(QQuickItem* parent)
    : QQuickItem(parent)
    , m_source(NULL)
    , m_sourceTextureProvider(NULL)
    , m_backgroundColor(qRgba(0, 0, 0, 0))
    , m_secondaryBackgroundColor(qRgba(0, 0, 0, 0))
    , m_sourceScale(1.0f, 1.0f)
    , m_sourceTranslation(0.0f, 0.0f)
    , m_sourceTransform(1.0f, 1.0f, 0.0f, 0.0f)
    , m_radius(SmallRadius)
    , m_border(IdleBorder)
    , m_imageHorizontalAlignment(AlignHCenter)
    , m_imageVerticalAlignment(AlignVCenter)
    , m_backgroundMode(SolidColor)
    , m_sourceHorizontalAlignment(AlignHCenter)
    , m_sourceVerticalAlignment(AlignVCenter)
    , m_sourceFillMode(Stretch)
    , m_sourceHorizontalWrapMode(Transparent)
    , m_sourceVerticalWrapMode(Transparent)
    , m_sourceOpacity(255)
    , m_flags(Stretched)
{
    setFlag(ItemHasContents);
    QObject::connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()), this,
                     SLOT(_q_gridUnitChanged()));
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
    if (m_radius != newRadius) {
        m_radius = newRadius;
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
    Border border;
    if (borderSource.endsWith(QString("radius_idle.sci"))) {
        border = IdleBorder;
    } else if (borderSource.endsWith(QString("radius_pressed.sci"))) {
        border = PressedBorder;
    } else {
        border = RawBorder;
    }
    if (m_border != border) {
        m_border = border;
        update();
        Q_EMIT borderSourceChanged();
    }
}

// Deprecation layer.
void UCUbuntuShape::dropImageSupport()
{
    if (!(m_flags & SourceApiSet)) {
        m_flags |= SourceApiSet;
        if (m_source) {
            QObject::disconnect(m_source);
            m_source = NULL;
            update();
            Q_EMIT imageChanged();
        }
    }
}

/*! \qmlproperty variant UbuntuShape::source
    \since Ubuntu.Components 1.2

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
    if (m_source != newSource) {
        if (newSource) {
            if (!newSource->parentItem()) {
                // Inlined images need a parent and must not be visible.
                newSource->setParentItem(this);
                newSource->setVisible(false);
            }
            m_flags |= DirtySourceTransform;
        }
        m_source = newSource;
        update();
        Q_EMIT sourceChanged();
    }
}

/*! \qmlproperty real UbuntuShape::sourceOpacity
    \since Ubuntu.Components 1.2

    This property holds the opacity of the \l source texture. Opacity is specified as a number
    between 0.0 (fully transparent) and 1.0 (fully opaque). The default value is 1.0.

    \note Setting this disables support for the deprecated \l image property.
*/
void UCUbuntuShape::setSourceOpacity(qreal sourceOpacity)
{
    dropImageSupport();

    const quint8 sourceOpacityPacked = qBound(0.0, sourceOpacity, 1.0) * static_cast<qreal>(0xff);
    if (m_sourceOpacity != sourceOpacityPacked) {
        m_sourceOpacity = sourceOpacityPacked;
        update();
        Q_EMIT sourceOpacityChanged();
    }
}

/*! \qmlproperty enumeration UbuntuShape::sourceFillMode
    \since Ubuntu.Components 1.2

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

    if (m_sourceFillMode != sourceFillMode) {
        m_sourceFillMode = sourceFillMode;
        m_flags |= DirtySourceTransform;
        update();
        Q_EMIT sourceFillModeChanged();
    }
}

/*! \qmlproperty enumeration UbuntuShape::sourceHorizontalWrapMode
    \qmlproperty enumeration UbuntuShape::sourceVerticalWrapMode
    \since Ubuntu.Components 1.2

    When the \l sourceFillMode is set to \c UbuntuShape.Pad or \c UbuntuShape.PreserveAspectFit or
    when the \l sourceScale and/or \l sourceTranslation properties are changed, the \l source
    texture might not cover the entire UbuntuShape area. This property defines how the source
    texture wraps outside of its content area. The default value is \c UbuntuShape.Transparent.

    Ensure \c UbuntuShape.Repeat is used only when necessary (in case of an \c Image or \c
    AnimatedImage source) as it requires the underlying texture to be extracted from its atlas. That
    slows down the rendering speed since it prevents the QtQuick renderer to batch the UbuntuShape
    with others.

    \note Some OpenGL ES 2 implementations do not support \c UbuntuShape.Repeat with
    non-power-of-two sized source textures.
    \note Setting this disables support for the deprecated \l image property.

    \list
    \li \b UbuntuShape.Transparent - the source is clamped to transparent
    \li \b UbuntuShape.Repeat - the source is repeated indefinitely
    \endlist
*/
void UCUbuntuShape::setSourceHorizontalWrapMode(UCUbuntuShape::WrapMode sourceHorizontalWrapMode)
{
    dropImageSupport();

    if (m_sourceHorizontalWrapMode != sourceHorizontalWrapMode) {
        m_sourceHorizontalWrapMode = sourceHorizontalWrapMode;
        update();
        Q_EMIT sourceHorizontalWrapModeChanged();
    }
}

void UCUbuntuShape::setSourceVerticalWrapMode(UCUbuntuShape::WrapMode sourceVerticalWrapMode)
{
    dropImageSupport();

    if (m_sourceVerticalWrapMode != sourceVerticalWrapMode) {
        m_sourceVerticalWrapMode = sourceVerticalWrapMode;
        update();
        Q_EMIT sourceVerticalWrapModeChanged();
    }
}

/*! \qmlproperty enumeration UbuntuShape::sourceHorizontalAlignment
    \since Ubuntu.Components 1.2

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

    if (m_sourceHorizontalAlignment != sourceHorizontalAlignment) {
        m_sourceHorizontalAlignment = sourceHorizontalAlignment;
        m_flags |= DirtySourceTransform;
        update();
        Q_EMIT sourceHorizontalAlignmentChanged();
    }
}

/*! \qmlproperty enumeration UbuntuShape::sourceVerticalAlignment
    \since Ubuntu.Components 1.2

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

    if (m_sourceVerticalAlignment != sourceVerticalAlignment) {
        m_sourceVerticalAlignment = sourceVerticalAlignment;
        m_flags |= DirtySourceTransform;
        update();
        Q_EMIT sourceVerticalAlignmentChanged();
    }
}

/*! \qmlproperty vector2d UbuntuShape::sourceTranslation
    \since Ubuntu.Components 1.2

    This property defines the two-component vector in normalized item coordinates used to translate
    the \l source texture. The default value is \c {Qt.vector2d(0.0,0.0)}.

    That can be used to put the \l source texture at a precise position, to create infinite
    scrolling animations (using the \c UbuntuShape.Repeat wrap mode), etc.

    \note Setting this disables support for the deprecated \l image property.
*/
void UCUbuntuShape::setSourceTranslation(const QVector2D& sourceTranslation)
{
    dropImageSupport();

    if (m_sourceTranslation != sourceTranslation) {
        m_sourceTranslation = sourceTranslation;
        m_flags |= DirtySourceTransform;
        update();
        Q_EMIT sourceTranslationChanged();
    }
}

/*! \qmlproperty vector2d UbuntuShape::sourceScale
    \since Ubuntu.Components 1.2

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

    if (m_sourceScale != sourceScale) {
        m_sourceScale = sourceScale;
        m_flags |= DirtySourceTransform;
        update();
        Q_EMIT sourceScaleChanged();
    }
}

// Deprecation layer.
void UCUbuntuShape::dropColorSupport()
{
    if (!(m_flags & BackgroundApiSet)) {
        m_flags |= BackgroundApiSet;
        if (m_backgroundColor) {
            m_backgroundColor = qRgba(0, 0, 0, 0);
            Q_EMIT colorChanged();
        }
        if (m_secondaryBackgroundColor) {
            m_secondaryBackgroundColor = qRgba(0, 0, 0, 0);
            Q_EMIT gradientColorChanged();
        }
    }
}

/*! \qmlproperty color UbuntuShape::backgroundColor
    \since Ubuntu.Components 1.2

    This property defines the background color. The default value is transparent black.

    \note Setting this disables support for the deprecated \l color and \l gradientColor properties.
*/
void UCUbuntuShape::setBackgroundColor(const QColor& backgroundColor)
{
    dropColorSupport();

    const QRgb backgroundColorRgb = qRgba(
        backgroundColor.red(), backgroundColor.green(), backgroundColor.blue(),
        backgroundColor.alpha());
    if (m_backgroundColor != backgroundColorRgb) {
        m_backgroundColor = backgroundColorRgb;
        update();
        Q_EMIT backgroundColorChanged();
    }
}

/*! \qmlproperty color UbuntuShape::secondaryBackgroundColor
    \since Ubuntu.Components 1.2

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
    if (m_secondaryBackgroundColor != secondaryBackgroundColorRgb) {
        m_secondaryBackgroundColor = secondaryBackgroundColorRgb;
        update();
        Q_EMIT secondaryBackgroundColorChanged();
    }
}

/*! \qmlproperty enumeration UbuntuShape::backgroundMode
    \since Ubuntu.Components 1.2

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

    if (m_backgroundMode != backgroundMode) {
        m_backgroundMode = backgroundMode;
        update();
        Q_EMIT backgroundModeChanged();
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
    if (!(m_flags & BackgroundApiSet)) {
        const QRgb colorRgb = qRgba(color.red(), color.green(), color.blue(), color.alpha());
        if (m_backgroundColor != colorRgb) {
            m_backgroundColor = colorRgb;
            // gradientColor has the same value as color unless it was explicitly set.
            if (!(m_flags & GradientColorSet)) {
                m_secondaryBackgroundColor = colorRgb;
                Q_EMIT gradientColorChanged();
            }
            update();
            Q_EMIT colorChanged();
        }
    }
}

/*! \qmlproperty color UbuntuShape::gradientColor
    \deprecated

    This property defines the bottom color used for the vertical gradient filling the UbuntuShape
    when there is no \l image set. As long as this property is not set, a single color (defined
    by \l color) is used to fill the UbuntuShape.

    \note Use \l backgroundColor, \l secondaryBackgroundColor and \l backgroundMode instead.
*/
void UCUbuntuShape::setGradientColor(const QColor& gradientColor)
{
    if (!(m_flags & BackgroundApiSet)) {
        m_flags |= GradientColorSet;
        const QRgb gradientColorRgb = qRgba(
            gradientColor.red(), gradientColor.green(), gradientColor.blue(),
            gradientColor.alpha());
        if (m_secondaryBackgroundColor != gradientColorRgb) {
            m_secondaryBackgroundColor = gradientColorRgb;
            update();
            Q_EMIT gradientColorChanged();
        }
    }
}

/*! \qmlproperty Image UbuntuShape::image
    \deprecated

    This property holds the \c Image or \c ShaderEffectSource rendered in the UbuntuShape. In case
    of an \c Image, it watches for fillMode (\c Image.Stretch and\c Image.PreserveAspectCrop), \c
    horizontalAlignment and \c verticalAlignment property changes. The default value is \c null.

    \note Use \l source instead.
*/
void UCUbuntuShape::setImage(const QVariant& image)
{
    if (!(m_flags & SourceApiSet)) {
        QQuickItem* newImage = qobject_cast<QQuickItem*>(qvariant_cast<QObject*>(image));
        if (m_source != newImage) {
            if (newImage) {
                // Watch for property changes.
                updateFromImageProperties(newImage);
                connectToImageProperties(newImage);
                if (!newImage->parentItem()) {
                    // Inlined images need a parent and must not be visible.
                    newImage->setParentItem(this);
                    newImage->setVisible(false);
                }
                m_flags |= DirtySourceTransform;
            }
            QObject::disconnect(m_source);
            update();
            m_source = newImage;
            Q_EMIT imageChanged();
        }
    }
}

// Deprecation layer. Even though "stretched" is exposed as a QML property, it's only been used when
// there was a QML UbuntuShape proxy. This is why we don't provide doc for it. We'll still have to
// maintain it for a while for compatibility reasons.
void UCUbuntuShape::setStretched(bool stretched)
{
    if (!(m_flags & SourceApiSet)) {
        if (!!(m_flags & Stretched) != stretched) {
            if (stretched) {
                m_flags |= Stretched;
            } else {
                m_flags &= ~Stretched;
            }
            m_flags |= DirtySourceTransform;
            update();
            Q_EMIT stretchedChanged();
        }
    }
}

// Deprecation layer. Same comment as setStretched().
void UCUbuntuShape::setHorizontalAlignment(HAlignment horizontalAlignment)
{
    if (!(m_flags & SourceApiSet)) {
        if (m_imageHorizontalAlignment != horizontalAlignment) {
            m_imageHorizontalAlignment = horizontalAlignment;
            m_flags |= DirtySourceTransform;
            update();
            Q_EMIT horizontalAlignmentChanged();
        }
    }
}

// Deprecation layer. Same comment as setStretched().
void UCUbuntuShape::setVerticalAlignment(VAlignment verticalAlignment)
{
    if (!(m_flags & SourceApiSet)) {
        if (m_imageVerticalAlignment != verticalAlignment) {
            m_imageVerticalAlignment = verticalAlignment;
            m_flags |= DirtySourceTransform;
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
void UCUbuntuShape::connectToPropertyChange(
    QObject* sender, const char* property, QObject* receiver, const char* slot)
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
    connectToPropertyChange(image, "fillMode", this, "_q_imagePropertiesChanged()");
    connectToPropertyChange(image, "horizontalAlignment", this, "_q_imagePropertiesChanged()");
    connectToPropertyChange(image, "verticalAlignment", this, "_q_imagePropertiesChanged()");
}

// Deprecation layer.
void UCUbuntuShape::_q_imagePropertiesChanged()
{
    QQuickItem* image = qobject_cast<QQuickItem*>(sender());
    updateFromImageProperties(image);
}

void UCUbuntuShape::_q_openglContextDestroyed()
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

void UCUbuntuShape::_q_gridUnitChanged()
{
    const float gridUnit = UCUnits::instance().gridUnit();
    setImplicitWidth(implicitGridUnitWidth * gridUnit);
    setImplicitHeight(implicitGridUnitHeight * gridUnit);
    update();
}

void UCUbuntuShape::_q_providerDestroyed(QObject* object)
{
    Q_UNUSED(object);
    m_sourceTextureProvider = NULL;
}

void UCUbuntuShape::_q_textureChanged()
{
    m_flags |= DirtySourceTransform;
    update();
}

void UCUbuntuShape::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    m_flags |= DirtySourceTransform;
}

// Gets the nearest boundary to coord in the texel grid of the given size.
static Q_DECL_CONSTEXPR float roundTextureCoord(float coord, float size)
{
    return roundf(coord * size) / size;
}

void UCUbuntuShape::updateSourceTransform(
    float itemWidth, float itemHeight, FillMode fillMode, HAlignment horizontalAlignment,
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

    const float sourceSxInv = 1.0f / m_sourceScale.x();
    const float sourceSyInv = 1.0f / m_sourceScale.y();
    // Multiplied by fillS* so that the translation unit is in normalized item coordinates.
    const float sourceTx = (m_sourceTranslation.x() * sourceSxInv) * fillSx;
    const float sourceTy = (m_sourceTranslation.y() * sourceSyInv) * fillSy;
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
    m_sourceTransform = QVector4D(
        sx, sy, roundTextureCoord(tx, textureSize.width() * m_sourceScale.x()),
        roundTextureCoord(ty, textureSize.height() * m_sourceScale.y()));
}

// Pack a premultiplied 32-bit ABGR integer.
static quint32 packColor(quint32 a, quint32 b, quint32 g, quint32 r)
{
    const quint32 pb = ((b * a) + 0xff) >> 8;
    const quint32 pg = ((g * a) + 0xff) >> 8;
    const quint32 pr = ((r * a) + 0xff) >> 8;
    return (a << 24) | ((pb & 0xff) << 16) | ((pg & 0xff) << 8) | (pr & 0xff);
}

// Lerp c1 and c2 with t in the range [0, 255]. Return value is a premultiplied 32-bit ABGR integer.
static quint32 lerpColor(quint32 t, QRgb c1, QRgb c2)
{
    const quint32 a = qAlpha(c1) + ((t * (qAlpha(c2) - qAlpha(c1)) + 0xff) >> 8);
    const quint32 b = qBlue(c1) + ((t * (qBlue(c2) - qBlue(c1)) + 0xff) >> 8);
    const quint32 g = qGreen(c1) + ((t * (qGreen(c2) - qGreen(c1)) + 0xff) >> 8);
    const quint32 r = qRed(c1) + ((t * (qRed(c2) - qRed(c1)) + 0xff) >> 8);
    return packColor(a, b, g, r);
}

QSGNode* UCUbuntuShape::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data)
{
    Q_UNUSED(data);

    const QSizeF itemSize(width(), height());
    if (itemSize.isEmpty()) {
        delete oldNode;
        return NULL;
    }

    QSGNode* node = oldNode ? oldNode : createSceneGraphNode();
    Q_ASSERT(node);

    // OpenGL allocates textures per context, so we store textures reused by all shape instances
    // per context as well.
    QOpenGLContext* openglContext = window() ? window()->openglContext() : NULL;
    Q_ASSERT(openglContext);
    ShapeTextures &shapeTextures = shapeTexturesHash[openglContext];
    if (!shapeTextures.high) {
        shapeTextures.high = window()->createTextureFromImage(
            QImage(shapeTextureHigh.data, shapeTextureHigh.width, shapeTextureHigh.height,
                   QImage::Format_ARGB32_Premultiplied));
        shapeTextures.low = window()->createTextureFromImage(
            QImage(shapeTextureLow.data, shapeTextureLow.width, shapeTextureLow.height,
                   QImage::Format_ARGB32_Premultiplied));
        QObject::connect(openglContext, SIGNAL(aboutToBeDestroyed()),
                         this, SLOT(_q_openglContextDestroyed()), Qt::DirectConnection);
    }

    // Get the texture info and update the source transform if needed.
    QSGTextureProvider* provider = m_source ? m_source->textureProvider() : NULL;
    QSGTexture* sourceTexture = provider ? provider->texture() : NULL;
    QRectF sourceTextureRect(0.0f, 0.0f, 1.0f, 1.0f);
    if (sourceTexture) {
        if (m_sourceHorizontalWrapMode == Transparent && m_sourceVerticalWrapMode == Transparent) {
            sourceTextureRect = sourceTexture->normalizedTextureSubRect();
        }
        if (m_flags & DirtySourceTransform) {
            if (m_flags & SourceApiSet) {
                updateSourceTransform(itemSize.width(), itemSize.height(), m_sourceFillMode,
                                      m_sourceHorizontalAlignment, m_sourceVerticalAlignment,
                                      sourceTexture->textureSize());
            } else {
                FillMode imageFillMode = (m_flags & Stretched) ? Stretch : PreserveAspectCrop;
                updateSourceTransform(itemSize.width(), itemSize.height(), imageFillMode,
                                      m_imageHorizontalAlignment, m_imageVerticalAlignment,
                                      sourceTexture->textureSize());
            }
            m_flags &= ~DirtySourceTransform;
        }
    }

    // Update the shape item whenever the source item's texture changes.
    if (provider != m_sourceTextureProvider) {
        if (m_sourceTextureProvider) {
            QObject::disconnect(m_sourceTextureProvider, SIGNAL(textureChanged()),
                                this, SLOT(_q_textureChanged()));
            QObject::disconnect(m_sourceTextureProvider, SIGNAL(destroyed()),
                                this, SLOT(_q_providerDestroyed()));
        }
        if (provider) {
            QObject::connect(provider, SIGNAL(textureChanged()), this, SLOT(_q_textureChanged()));
            QObject::connect(provider, SIGNAL(destroyed()), this, SLOT(_q_providerDestroyed()));
        }
        m_sourceTextureProvider = provider;
    }

    const float gridUnit = UCUnits::instance().gridUnit();
    ShapeTextureData* shapeTextureData;
    QSGTexture* shapeTexture;
    if (gridUnit > lowHighTextureThreshold) {
        shapeTextureData = &shapeTextureHigh;
        shapeTexture = shapeTextures.high;
    } else {
        shapeTextureData = &shapeTextureLow;
        shapeTexture = shapeTextures.low;
    }

    // Set the shape texture to be used by the materials depending on current grid unit. The radius
    // is set considering the current grid unit and the texture raster grid unit. When the item size
    // is less than 2 radii, the radius is scaled down.
    QSGTexture::Filtering shapeTextureFiltering;
    float radius = (m_radius == SmallRadius) ?
        shapeTextureData->smallRadius : shapeTextureData->mediumRadius;
    const float scaleFactor = gridUnit / shapeTextureData->gridUnit;
    shapeTextureFiltering = QSGTexture::Nearest;
    if (scaleFactor != 1.0f) {
        radius *= scaleFactor;
        shapeTextureFiltering = QSGTexture::Linear;
    }
    const float halfMinWidthHeight = qMin(itemSize.width(), itemSize.height()) * 0.5f;
    if (radius > halfMinWidthHeight) {
        radius = halfMinWidthHeight;
        shapeTextureFiltering = QSGTexture::Linear;
    }

    updateMaterial(node, shapeTexture, shapeTextureFiltering, sourceTexture);

    // Select the right shape texture coordinates.
    int index = (m_border == RawBorder) ? 0 : (m_border == IdleBorder) ? 1 : 2;
    if (m_radius == SmallRadius) {
        index += 3;
    }

    // Get the affine transformation for the source texture coordinates.
    const QVector4D sourceCoordTransform(
        m_sourceTransform.x() * sourceTextureRect.width(),
        m_sourceTransform.y() * sourceTextureRect.height(),
        m_sourceTransform.z() * sourceTextureRect.width() + sourceTextureRect.x(),
        m_sourceTransform.w() * sourceTextureRect.height() + sourceTextureRect.y());

    // Get the affine transformation for the source mask coordinates, pixels lying inside the mask
    // (values in the range [-1, 1]) will be textured in the fragment shader. In case of a repeat
    // wrap mode, the transformation is made so that the mask takes the whole area.
    const QVector4D sourceMaskTransform(
        m_sourceHorizontalWrapMode == Transparent ? m_sourceTransform.x() * 2.0f : 2.0f,
        m_sourceVerticalWrapMode == Transparent ? m_sourceTransform.y() * 2.0f : 2.0f,
        m_sourceHorizontalWrapMode == Transparent ? m_sourceTransform.z() * 2.0f - 1.0f : -1.0f,
        m_sourceVerticalWrapMode == Transparent ? m_sourceTransform.w() * 2.0f - 1.0f : -1.0f);

    // Select and pack the lerp'd and premultiplied background colors.
    QRgb color[2];
    if (m_flags & BackgroundApiSet) {
        color[0] = m_backgroundColor;
        color[1] = (m_backgroundMode == SolidColor) ?
            m_backgroundColor : m_secondaryBackgroundColor;
    } else {
        if (!sourceTexture) {
            color[0] = m_backgroundColor;
            // For API compatibility reasons, m_secondaryBackgroundColor is set to m_backgroundColor
            // as long as setGradientColor() isn't called, so we can safely use it here.
            color[1] = m_secondaryBackgroundColor;
        } else {
            // The deprecated image API was created such that whenever an image is set, the
            // background color is transparent.
            color[0] = qRgba(0, 0, 0, 0);
            color[1] = qRgba(0, 0, 0, 0);
        }
    }
    const quint32 radiusHeight = static_cast<quint32>((radius / itemSize.height()) * 255.0f);
    const quint32 backgroundColor[4] = {
        packColor(qAlpha(color[0]), qBlue(color[0]), qGreen(color[0]), qRed(color[0])),
        lerpColor(radiusHeight, color[0], color[1]),
        lerpColor(255 - radiusHeight, color[0], color[1]),
        packColor(qAlpha(color[1]), qBlue(color[1]), qGreen(color[1]), qRed(color[1]))
    };

    updateGeometry(
        node, itemSize.width(), itemSize.height(), radius, shapeTextureData->coordinate[index],
        sourceCoordTransform, sourceMaskTransform, backgroundColor);

    return node;
}

QSGNode* UCUbuntuShape::createSceneGraphNode() const
{
    return new ShapeNode;
}

void UCUbuntuShape::updateMaterial(
    QSGNode* node, QSGTexture* shapeTexture, QSGTexture::Filtering shapeTextureFiltering,
    QSGTexture* sourceTexture)
{
    ShapeNode* shapeNode = static_cast<ShapeNode*>(node);
    ShapeMaterial::Data* materialData = shapeNode->material()->data();
    quint8 flags = 0;

    materialData->shapeTexture = shapeTexture;
    if (sourceTexture && m_sourceOpacity) {
        materialData->sourceTextureProvider = m_sourceTextureProvider;
        materialData->sourceOpacity = m_sourceOpacity;
        if (m_sourceHorizontalWrapMode == Repeat) {
            flags |= ShapeMaterial::Data::HorizontallyRepeated;
        }
        if (m_sourceVerticalWrapMode == Repeat) {
            flags |= ShapeMaterial::Data::VerticallyRepeated;
        }
        flags |= ShapeMaterial::Data::Textured;
    } else {
        materialData->sourceTextureProvider = NULL;
        materialData->sourceOpacity = 0;
    }
    materialData->shapeTextureFiltering = shapeTextureFiltering;
    materialData->flags = flags;
}

void UCUbuntuShape::updateGeometry(
    QSGNode* node, float width, float height, float radius, const float shapeCoordinate[][2],
    const QVector4D& sourceCoordTransform, const QVector4D& sourceMaskTransform,
    const quint32 backgroundColor[4])
{
    ShapeNode* shapeNode = static_cast<ShapeNode*>(node);
    ShapeNode::Vertex* v = reinterpret_cast<ShapeNode::Vertex*>(
        shapeNode->geometry()->vertexData());

    // Convert radius to normalized coordinates.
    const float rw = radius / width;
    const float rh = radius / height;

    // Set top row of 4 vertices.
    v[0].position[0] = 0.0f;
    v[0].position[1] = 0.0f;
    v[0].shapeCoordinate[0] = shapeCoordinate[0][0];
    v[0].shapeCoordinate[1] = shapeCoordinate[0][1];
    v[0].sourceCoordinate[0] = sourceCoordTransform.z();
    v[0].sourceCoordinate[1] = sourceCoordTransform.w();
    v[0].sourceCoordinate[2] = sourceMaskTransform.z();
    v[0].sourceCoordinate[3] = sourceMaskTransform.w();
    v[0].backgroundColor = backgroundColor[0];
    v[1].position[0] = radius;
    v[1].position[1] = 0.0f;
    v[1].shapeCoordinate[0] = shapeCoordinate[1][0];
    v[1].shapeCoordinate[1] = shapeCoordinate[1][1];
    v[1].sourceCoordinate[0] = rw * sourceCoordTransform.x() + sourceCoordTransform.z();
    v[1].sourceCoordinate[1] = sourceCoordTransform.w();
    v[1].sourceCoordinate[2] = rw * sourceMaskTransform.x() + sourceMaskTransform.z();
    v[1].sourceCoordinate[3] = sourceMaskTransform.w();
    v[1].backgroundColor = backgroundColor[0];
    v[2].position[0] = width - radius;
    v[2].position[1] = 0.0f;
    v[2].shapeCoordinate[0] = shapeCoordinate[2][0];
    v[2].shapeCoordinate[1] = shapeCoordinate[2][1];
    v[2].sourceCoordinate[0] = (1.0f - rw) * sourceCoordTransform.x() + sourceCoordTransform.z();
    v[2].sourceCoordinate[1] = sourceCoordTransform.w();
    v[2].sourceCoordinate[2] = (1.0f - rw) * sourceMaskTransform.x() + sourceMaskTransform.z();
    v[2].sourceCoordinate[3] = sourceMaskTransform.w();
    v[2].backgroundColor = backgroundColor[0];
    v[3].position[0] = width;
    v[3].position[1] = 0.0f;
    v[3].shapeCoordinate[0] = shapeCoordinate[3][0];
    v[3].shapeCoordinate[1] = shapeCoordinate[3][1];
    v[3].sourceCoordinate[0] = sourceCoordTransform.x() + sourceCoordTransform.z();
    v[3].sourceCoordinate[1] = sourceCoordTransform.w();
    v[3].sourceCoordinate[2] = sourceMaskTransform.x() + sourceMaskTransform.z();
    v[3].sourceCoordinate[3] = sourceMaskTransform.w();
    v[3].backgroundColor = backgroundColor[0];

    // Set middle-top row of 4 vertices.
    v[4].position[0] = 0.0f;
    v[4].position[1] = radius;
    v[4].shapeCoordinate[0] = shapeCoordinate[4][0];
    v[4].shapeCoordinate[1] = shapeCoordinate[4][1];
    v[4].sourceCoordinate[0] = sourceCoordTransform.z();
    v[4].sourceCoordinate[1] = rh * sourceCoordTransform.y() + sourceCoordTransform.w();
    v[4].sourceCoordinate[2] = sourceMaskTransform.z();
    v[4].sourceCoordinate[3] = rh * sourceMaskTransform.y() + sourceMaskTransform.w();
    v[4].backgroundColor = backgroundColor[1];
    v[5].position[0] = radius;
    v[5].position[1] = radius;
    v[5].shapeCoordinate[0] = shapeCoordinate[5][0];
    v[5].shapeCoordinate[1] = shapeCoordinate[5][1];
    v[5].sourceCoordinate[0] = rw * sourceCoordTransform.x() + sourceCoordTransform.z();
    v[5].sourceCoordinate[1] = rh * sourceCoordTransform.y() + sourceCoordTransform.w();
    v[5].sourceCoordinate[2] = rw * sourceMaskTransform.x() + sourceMaskTransform.z();
    v[5].sourceCoordinate[3] = rh * sourceMaskTransform.y() + sourceMaskTransform.w();
    v[5].backgroundColor = backgroundColor[1];
    v[6].position[0] = width - radius;
    v[6].position[1] = radius;
    v[6].shapeCoordinate[0] = shapeCoordinate[6][0];
    v[6].shapeCoordinate[1] = shapeCoordinate[6][1];
    v[6].sourceCoordinate[0] = (1.0f - rw) * sourceCoordTransform.x() + sourceCoordTransform.z();
    v[6].sourceCoordinate[1] = rh * sourceCoordTransform.y() + sourceCoordTransform.w();
    v[6].sourceCoordinate[2] = (1.0f - rw) * sourceMaskTransform.x() + sourceMaskTransform.z();
    v[6].sourceCoordinate[3] = rh * sourceMaskTransform.y() + sourceMaskTransform.w();
    v[6].backgroundColor = backgroundColor[1];
    v[7].position[0] = width;
    v[7].position[1] = radius;
    v[7].shapeCoordinate[0] = shapeCoordinate[7][0];
    v[7].shapeCoordinate[1] = shapeCoordinate[7][1];
    v[7].sourceCoordinate[0] = sourceCoordTransform.x() + sourceCoordTransform.z();
    v[7].sourceCoordinate[1] = rh * sourceCoordTransform.y() + sourceCoordTransform.w();
    v[7].sourceCoordinate[2] = sourceMaskTransform.x() + sourceMaskTransform.z();
    v[7].sourceCoordinate[3] = rh * sourceMaskTransform.y() + sourceMaskTransform.w();
    v[7].backgroundColor = backgroundColor[1];

    // Set middle-bottom row of 4 vertices.
    v[8].position[0] = 0.0f;
    v[8].position[1] = height - radius;
    v[8].shapeCoordinate[0] = shapeCoordinate[8][0];
    v[8].shapeCoordinate[1] = shapeCoordinate[8][1];
    v[8].sourceCoordinate[0] = sourceCoordTransform.z();
    v[8].sourceCoordinate[1] = (1.0f - rh) * sourceCoordTransform.y() + sourceCoordTransform.w();
    v[8].sourceCoordinate[2] = sourceMaskTransform.z();
    v[8].sourceCoordinate[3] = (1.0f - rh) * sourceMaskTransform.y() + sourceMaskTransform.w();
    v[8].backgroundColor = backgroundColor[2];
    v[9].position[0] = radius;
    v[9].position[1] = height - radius;
    v[9].shapeCoordinate[0] = shapeCoordinate[9][0];
    v[9].shapeCoordinate[1] = shapeCoordinate[9][1];
    v[9].sourceCoordinate[0] = rw * sourceCoordTransform.x() + sourceCoordTransform.z();
    v[9].sourceCoordinate[1] = (1.0f - rh) * sourceCoordTransform.y() + sourceCoordTransform.w();
    v[9].sourceCoordinate[2] = rw * sourceMaskTransform.x() + sourceMaskTransform.z();
    v[9].sourceCoordinate[3] = (1.0f - rh) * sourceMaskTransform.y() + sourceMaskTransform.w();
    v[9].backgroundColor = backgroundColor[2];
    v[10].position[0] = width - radius;
    v[10].position[1] = height - radius;
    v[10].shapeCoordinate[0] = shapeCoordinate[10][0];
    v[10].shapeCoordinate[1] = shapeCoordinate[10][1];
    v[10].sourceCoordinate[0] = (1.0f - rw) * sourceCoordTransform.x() + sourceCoordTransform.z();
    v[10].sourceCoordinate[1] = (1.0f - rh) * sourceCoordTransform.y() + sourceCoordTransform.w();
    v[10].sourceCoordinate[2] = (1.0f - rw) * sourceMaskTransform.x() + sourceMaskTransform.z();
    v[10].sourceCoordinate[3] = (1.0f - rh) * sourceMaskTransform.y() + sourceMaskTransform.w();
    v[10].backgroundColor = backgroundColor[2];
    v[11].position[0] = width;
    v[11].position[1] = height - radius;
    v[11].shapeCoordinate[0] = shapeCoordinate[11][0];
    v[11].shapeCoordinate[1] = shapeCoordinate[11][1];
    v[11].sourceCoordinate[0] = sourceCoordTransform.x() + sourceCoordTransform.z();
    v[11].sourceCoordinate[1] = (1.0f - rh) * sourceCoordTransform.y() + sourceCoordTransform.w();
    v[11].sourceCoordinate[2] = sourceMaskTransform.x() + sourceMaskTransform.z();
    v[11].sourceCoordinate[3] = (1.0f - rh) * sourceMaskTransform.y() + sourceMaskTransform.w();
    v[11].backgroundColor = backgroundColor[2];

    // Set bottom row of 4 vertices.
    v[12].position[0] = 0.0f;
    v[12].position[1] = height;
    v[12].shapeCoordinate[0] = shapeCoordinate[12][0];
    v[12].shapeCoordinate[1] = shapeCoordinate[12][1];
    v[12].sourceCoordinate[0] = sourceCoordTransform.z();
    v[12].sourceCoordinate[1] = sourceCoordTransform.y() + sourceCoordTransform.w();
    v[12].sourceCoordinate[2] = sourceMaskTransform.z();
    v[12].sourceCoordinate[3] = sourceMaskTransform.y() + sourceMaskTransform.w();
    v[12].backgroundColor = backgroundColor[3];
    v[13].position[0] = radius;
    v[13].position[1] = height;
    v[13].shapeCoordinate[0] = shapeCoordinate[13][0];
    v[13].shapeCoordinate[1] = shapeCoordinate[13][1];
    v[13].sourceCoordinate[0] = rw * sourceCoordTransform.x() + sourceCoordTransform.z();
    v[13].sourceCoordinate[1] = sourceCoordTransform.y() + sourceCoordTransform.w();
    v[13].sourceCoordinate[2] = rw * sourceMaskTransform.x() + sourceMaskTransform.z();
    v[13].sourceCoordinate[3] = sourceMaskTransform.y() + sourceMaskTransform.w();
    v[13].backgroundColor = backgroundColor[3];
    v[14].position[0] = width - radius;
    v[14].position[1] = height;
    v[14].shapeCoordinate[0] = shapeCoordinate[14][0];
    v[14].shapeCoordinate[1] = shapeCoordinate[14][1];
    v[14].sourceCoordinate[0] = (1.0f - rw) * sourceCoordTransform.x() + sourceCoordTransform.z();
    v[14].sourceCoordinate[1] = sourceCoordTransform.y() + sourceCoordTransform.w();
    v[14].sourceCoordinate[2] = (1.0f - rw) * sourceMaskTransform.x() + sourceMaskTransform.z();
    v[14].sourceCoordinate[3] = sourceMaskTransform.y() + sourceMaskTransform.w();
    v[14].backgroundColor = backgroundColor[3];
    v[15].position[0] = width;
    v[15].position[1] = height;
    v[15].shapeCoordinate[0] = shapeCoordinate[15][0];
    v[15].shapeCoordinate[1] = shapeCoordinate[15][1];
    v[15].sourceCoordinate[0] = sourceCoordTransform.x() + sourceCoordTransform.z();
    v[15].sourceCoordinate[1] = sourceCoordTransform.y() + sourceCoordTransform.w();
    v[15].sourceCoordinate[2] = sourceMaskTransform.x() + sourceMaskTransform.z();
    v[15].sourceCoordinate[3] = sourceMaskTransform.y() + sourceMaskTransform.w();
    v[15].backgroundColor = backgroundColor[3];

    node->markDirty(QSGNode::DirtyGeometry);
}
