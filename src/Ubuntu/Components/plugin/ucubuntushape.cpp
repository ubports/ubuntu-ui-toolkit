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

// The UbuntuShape uses a simple and efficient method described by Chris Green in this paper from
// 2007 (http://www.valvesoftware.com/publications/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf)
// to create its anti-aliased and resolution independent contour.

// FIXME(loicm) Storing lower precision data types in the vertex buffer could be more efficent. Note
//     that on PowerVR GPUs (and certainly others), it requires a conversion in the USSE pipeline so
//     the trade-off between shader cycles and bandwidth requirements needs to be precisely
//     evaluated.

#include "ucubuntushape.h"
#include "ucubuntushapetexture.h"
#include "ucunits.h"
#include <QtCore/QPointer>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGTextureProvider>
#include <QtQuick/private/qquickimage_p.h>
#include <QtQuick/private/qsgadaptationlayer_p.h>
#include <QtQml/qqmlinfo.h>
#include <math.h>

// Anti-aliasing distance of the contour in pixels.
const float distanceAApx = 1.75f;

// For cosmetic reasons, we add an offset to the radius size to avoid having values less than 2 to
// look as if it has no rounded corners.
const float radiusSizeOffset = 2.0f;

// Factor by which the final fragment RGB color must be multiplied for the pressed aspect.
const float pressedFactor = 0.85f;

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
    m_dfdtFactorsId = program()->uniformLocation("dfdtFactors");
    m_opacityFactorsId = program()->uniformLocation("opacityFactors");
    m_sourceOpacityId = program()->uniformLocation("sourceOpacity");
    m_distanceAAId = program()->uniformLocation("distanceAA");
    m_texturedId = program()->uniformLocation("textured");
    m_aspectId = program()->uniformLocation("aspect");
}

void ShapeShader::updateState(
    const RenderState& state, QSGMaterial* newEffect, QSGMaterial* oldEffect)
{
    Q_UNUSED(oldEffect);

    const ShapeMaterial::Data* data = static_cast<ShapeMaterial*>(newEffect)->constData();

    // Bind shape texture.
    glBindTexture(GL_TEXTURE_2D, data->shapeTexture);

    // Bind source texture on the 2nd texture unit and update uniforms.
    bool textured = false;
    if (data->flags & ShapeMaterial::Data::Textured) {
        QSGTextureProvider* provider = data->sourceTextureProvider;
        QSGTexture* sourceTexture = provider ? provider->texture() : NULL;
        if (sourceTexture) {
            if (data->flags & ShapeMaterial::Data::Repeated) {
                if (sourceTexture->isAtlasTexture()) {
                    // A texture in an atlas can't be repeated with builtin GPU facility (exposed by
                    // GL_REPEAT with OpenGL), so we extract it and create a new dedicated one.
                    sourceTexture = sourceTexture->removedFromAtlas();
                }
                sourceTexture->setHorizontalWrapMode(
                    data->flags & ShapeMaterial::Data::HorizontallyRepeated ?
                    QSGTexture::Repeat : QSGTexture::ClampToEdge);
                sourceTexture->setVerticalWrapMode(
                    data->flags & ShapeMaterial::Data::VerticallyRepeated ?
                    QSGTexture::Repeat : QSGTexture::ClampToEdge);
            }
            m_functions->glActiveTexture(GL_TEXTURE1);
            sourceTexture->bind();
            m_functions->glActiveTexture(GL_TEXTURE0);
            program()->setUniformValue(m_sourceOpacityId, data->sourceOpacity / 255.0f);
            textured = true;
        }
    }
    program()->setUniformValue(m_texturedId, textured);
    program()->setUniformValue(
        m_aspectId, data->flags & (ShapeMaterial::Data::Flat | ShapeMaterial::Data::Inset));

    // The pressed aspect is implemented by scaling the final RGB fragment color. It's not a real
    // blending as it was done before deprecation, so for instance transparent colors remain the
    // same, but we consider it would be too costly to maintain for a deprecated feature that was
    // actually only used in the toolkit and never documented. The factor is multiplied with the Qt
    // opacity to avoid useless operations in the shader.
    const float opacity = state.opacity();
    const QVector2D opacityFactorsVector(
        data->flags & ShapeMaterial::Data::Pressed ? pressedFactor * opacity : opacity, opacity);
    program()->setUniformValue(m_opacityFactorsId, opacityFactorsVector);

    // Send anti-aliasing distance in distance field space, needs to be divided by 2 for the shader
    // and by 255 for distanceAAFactor dequantization. The factor is 1 most of the time apart when
    // the radius size is low, it linearly goes from 1 to 0 to make the corners prettier and to
    // prevent the opacity of the whole shape to slightly lower.
    const float distanceAA = (shapeTextureInfo.distanceAA * distanceAApx) / (2.0 * 255.0f);
    program()->setUniformValue(m_distanceAAId, data->distanceAAFactor * distanceAA);

    // Send screen-space derivative factors. Note that when rendering is redirected to a
    // ShaderEffectSource (FBO), dFdy() sign is flipped.
    const float orientation = static_cast<float>(data->dfdtFactors & 0x4);
    const float flip = static_cast<float>(data->dfdtFactors & 0x3) - 1.0f;
    const bool flipped = orientation != 1.0f && state.projectionMatrix()(1, 3) < 0.0f;
    const QVector2D dfdtFactors(orientation, flipped ? -flip : flip);
    program()->setUniformValue(m_dfdtFactorsId, dfdtFactors);

    // Update QtQuick engine uniforms.
    if (state.isMatrixDirty()) {
        program()->setUniformValue(m_matrixId, state.combinedMatrix());
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

void ShapeMaterial::updateTextures()
{
    if (m_data.flags & ShapeMaterial::Data::Textured && m_data.sourceTextureProvider) {
        if (QSGLayer* texture = qobject_cast<QSGLayer*>(m_data.sourceTextureProvider->texture())) {
            texture->updateTexture();
        }
    }
}

// --- Scene graph node ---

ShapeNode::ShapeNode()
    : QSGGeometryNode()
    , m_material()
    , m_geometry(attributeSet(), vertexCount, indexCount, indexType)
{
    QSGNode::setFlag(UsePreprocess, true);
    memcpy(m_geometry.indexData(), indices(), indexCount * indexTypeSize);
    m_geometry.setDrawingMode(drawingMode);
    m_geometry.setIndexDataPattern(indexDataPattern);
    m_geometry.setVertexDataPattern(vertexDataPattern);
    setMaterial(&m_material);
    setGeometry(&m_geometry);
#ifdef QSG_RUNTIME_DESCRIPTION
    qsgnode_set_description(this, QLatin1String("ubuntushape"));
#endif
}

// static
const unsigned short* ShapeNode::indices()
{
    // The geometry is made of 9 vertices indexed with a triangle strip mode.
    // 0 - 1 - 2
    // | / | / |
    // 3 - 4 - 5
    // | / | / |
    // 6 - 7 - 8
    static const unsigned short indices[] = {
        0, 3, 1, 4, 2, 5,
        5, 3,  // Degenerate triangles.
        3, 6, 4, 7, 5, 8
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

static QHash<QOpenGLContext*, quint32> shapeTextureHash;
static bool isPrimaryOrientationLandscape = false;

const float implicitWidthGU = 8.0f;
const float implicitHeightGU = 8.0f;
const float radiusGuMap[3] = { 1.45f, 2.55f, 3.65f };

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
    , m_radius(Small)
    , m_aspect(Inset)
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
    static bool once = true;
    if (once) {
        // Stored statically as the primary orientation is fixed and we don't support multiple
        // screens for now.
        if (QGuiApplication::primaryScreen()->primaryOrientation() &
            (Qt::LandscapeOrientation | Qt::InvertedLandscapeOrientation)) {
            isPrimaryOrientationLandscape = true;
        }
        once = false;
    }

    setFlag(ItemHasContents);
    QObject::connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()), this,
                     SLOT(_q_gridUnitChanged()));
    const float gridUnit = UCUnits::instance().gridUnit();
    setImplicitWidth(implicitWidthGU * gridUnit);
    setImplicitHeight(implicitHeightGU * gridUnit);
    update();
}

/*! \qmlproperty string UbuntuShape::radius

    This property defines the corner radius. Three fixed values are supported: \c "small",
    \c "medium" and \c "large". The default value is \c "small".
*/
void UCUbuntuShape::setRadius(const QString& radius)
{
  const Radius newRadius = (radius == "medium") ? Medium : ((radius == "large") ? Large : Small);
    if (m_radius != newRadius) {
        m_radius = newRadius;
        update();
        Q_EMIT radiusChanged();
    }
}

/*! \qmlproperty enumeration UbuntuShape::aspect

    This property defines the graphical style of the UbuntuShape. The default value is \c
    UbuntuShape.Inset.

    \note Setting this disables support for the deprecated \l borderSource property. Use the
    UbuntuShapeOverlay item in order to provide the inset "pressed" aspect previously supported by
    that property.

    \list
    \li \b UbuntuShape.Flat - no effects applied
    \li \b UbuntuShape.Inset - inner shadow slightly moved downwards and bevelled bottom
    \endlist
*/
void UCUbuntuShape::setAspect(Aspect aspect)
{
    if (!(m_flags & AspectSet)) {
        m_flags |= AspectSet;
        m_aspect = Flat;
        update();
        Q_EMIT borderSourceChanged();
    }

    const quint8 newAspect = aspect;
    if (m_aspect != newAspect) {
        m_aspect = newAspect;
        update();
        Q_EMIT aspectChanged();
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

/*! \qmlproperty string UbuntuShape::borderSource
    \deprecated

    This property defines the look of the shape borders. The supported strings are \c
    "radius_idle.sci" providing an idle button aspect and \c "radius_pressed.sci" providing a
    pressed button aspect. Any other strings (like the empty one \c "") provides a shape with no
    borders. The default value is \c "radius_idle.sci".

    \note Use \l aspect instead.
*/
void UCUbuntuShape::setBorderSource(const QString& borderSource)
{
    // FIXME(loicm) Commented for now since even if it's deprecated, buttons have to use it in order
    //     to get a pressed aspect. This is a temporary solution before using a dedicated button
    //     shape.
    // qmlInfo(this) << "'borderSource' is deprecated. Use 'aspect' instead.";

    if (!(m_flags & AspectSet)) {
        quint8 aspect;
        if (borderSource.endsWith(QString("radius_idle.sci"))) {
            aspect = Inset;
        } else if (borderSource.endsWith(QString("radius_pressed.sci"))) {
            aspect = Pressed;
        } else {
            aspect = Flat;
        }
        if (m_aspect != aspect) {
            m_aspect = aspect;
            update();
            Q_EMIT borderSourceChanged();
        }
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
    qmlInfo(this) << "'color' is deprecated. Use 'backgroundColor', 'secondaryBackgroundColor' and "
        "'backgroundMode' instead.";

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
    qmlInfo(this) << "'gradientColor' is deprecated. Use 'backgroundColor', "
        "'secondaryBackgroundColor' and 'backgroundMode' instead.";

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
    qmlInfo(this) << "'image' is deprecated. Use 'source' instead.";

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
    qmlInfo(this) << "'stretched' is deprecated. Use 'sourceFillMode' instead";

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
    qmlInfo(this) << "'horizontalAlignment' is deprecated. Use 'sourceHorizontalAlignment' instead";

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
    qmlInfo(this) << "'horizontalAlignment' is deprecated. Use 'sourceVerticalAlignment' instead";

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
    updateFromImageProperties(qobject_cast<QQuickItem*>(sender()));
}

void UCUbuntuShape::_q_openglContextDestroyed()
{
    // Delete the shape texture that's stored per context and shared by all the shape items.
    quint32 shapeTexture = shapeTextureHash.take(qobject_cast<QOpenGLContext*>(sender()));
    glDeleteTextures(1, &shapeTexture);
}

void UCUbuntuShape::_q_gridUnitChanged()
{
    const float gridUnit = UCUnits::instance().gridUnit();
    setImplicitWidth(implicitWidthGU * gridUnit);
    setImplicitHeight(implicitHeightGU * gridUnit);
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

// Average c1 and c2. Return value is a premultiplied 32-bit ABGR integer.
static quint32 averageColor(QRgb c1, QRgb c2)
{
    return packColor(
        (qAlpha(c1) + qAlpha(c2)) >> 1, (qBlue(c1) + qBlue(c2)) >> 1,
        (qGreen(c1) + qGreen(c2)) >> 1, (qRed(c1) + qRed(c2)) >> 1);
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

    // Get or create the shape texture that's stored per context and shared by all the shape items.
    Q_ASSERT(window());
    QOpenGLContext* openglContext = window()->openglContext();
    Q_ASSERT(openglContext);
    quint32 shapeTexture = shapeTextureHash[openglContext];
    if (!shapeTexture) {
        glGenTextures(1, &shapeTexture);
        glBindTexture(GL_TEXTURE_2D, shapeTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, shapeTextureInfo.size, shapeTextureInfo.size, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, shapeTextureInfo.data);
        shapeTextureHash[openglContext] = shapeTexture;
        QObject::connect(openglContext, SIGNAL(aboutToBeDestroyed()), this,
                         SLOT(_q_openglContextDestroyed()), Qt::DirectConnection);
    }

    // Get the source texture info and update the source transform if needed.
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

    // Ensure the shape item is updated whenever the source item's texture changes.
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

    // Get the radius size. When the item width and/or height is less than 2 * radius, the size is
    // scaled down accordingly. The shape was using a fixed image for the corner before switching to
    // a distance field, since the corner wasn't taking the whole image (ending at ~80%) we need
    // to take that into account when the size is scaled down.
    float radius = UCUnits::instance().gridUnit() * radiusGuMap[m_radius];
    const float scaledDownRadius = qMin(itemSize.width(), itemSize.height()) * 0.5f * 0.8f;
    if (radius > scaledDownRadius) {
        radius = scaledDownRadius;
    }

    updateMaterial(node, radius, shapeTexture, sourceTexture && m_sourceOpacity);

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

    // Select and pack the lerped and premultiplied background colors.
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
    const quint32 backgroundColor[3] = {
        packColor(qAlpha(color[0]), qBlue(color[0]), qGreen(color[0]), qRed(color[0])),
        averageColor(color[0], color[1]),
        packColor(qAlpha(color[1]), qBlue(color[1]), qGreen(color[1]), qRed(color[1]))
    };

    updateGeometry(
        node, itemSize, radius, shapeTextureInfo.offset, sourceCoordTransform, sourceMaskTransform,
        backgroundColor);

    return node;
}

QSGNode* UCUbuntuShape::createSceneGraphNode() const
{
    return new ShapeNode;
}

void UCUbuntuShape::updateMaterial(QSGNode* node, float radius, quint32 shapeTexture, bool textured)
{
    ShapeMaterial::Data* materialData = static_cast<ShapeNode*>(node)->material()->data();
    quint8 flags = 0;

    materialData->shapeTexture = shapeTexture;
    if (textured) {
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

    // Mapping of radius size range from [0, 4] to [0, 1] with clamping, plus quantization.
    const float start = 0.0f + radiusSizeOffset;
    const float end = 4.0f + radiusSizeOffset;
    materialData->distanceAAFactor = qMin(
        (radius / (end - start)) - (start / (end - start)), 1.0f) * 255.0f;

    // Screen-space derivatives factors for fragment shaders depend on the primary orientation and
    // content orientation. A flag indicating a 90° rotation around the primary orientation is
    // stored on the 3rd bit of dfdtFactors, the flip factor is stored on the first 2 bits as 0 for
    // -1 and as 2 for 1 (efficiently converted using: float(x & 0x3) - 1.0f).
    const Qt::ScreenOrientation contentOrientation = window()->contentOrientation();
    if (isPrimaryOrientationLandscape) {
        const quint8 portraitMask = Qt::PortraitOrientation | Qt::InvertedPortraitOrientation;
        const quint8 flipMask = Qt::InvertedLandscapeOrientation | Qt::InvertedPortraitOrientation;
        quint8 factors = contentOrientation & portraitMask ? 0x4 : 0x0;
        factors |= contentOrientation & flipMask ? 0x0 : 0x2;
        materialData->dfdtFactors = factors;
    } else {
        const quint8 landscapeMask = Qt::LandscapeOrientation | Qt::InvertedLandscapeOrientation;
        const quint8 flipMask = Qt::InvertedPortraitOrientation | Qt::LandscapeOrientation;
        quint8 factors = contentOrientation & landscapeMask ? 0x4 : 0x0;
        factors |= contentOrientation & flipMask ? 0x0 : 0x2;
        materialData->dfdtFactors = factors;
    }

    // When the radius is equal to radiusSizeOffset (which means radius size is 0), no aspect is
    // flagged so that a dedicated (statically flow controlled) shaved off shader can be used for
    // optimal performance.
    if (radius > radiusSizeOffset) {
        const quint8 aspectFlags[] = {
            ShapeMaterial::Data::Flat, ShapeMaterial::Data::Inset,
            ShapeMaterial::Data::Inset | ShapeMaterial::Data::Pressed
        };
        flags |= aspectFlags[m_aspect];
    } else {
        const quint8 aspectFlags[] = { 0, 0, ShapeMaterial::Data::Pressed };
        flags |= aspectFlags[m_aspect];
    }

    materialData->flags = flags;
}

void UCUbuntuShape::updateGeometry(
    QSGNode* node, const QSizeF& itemSize, float radius, float shapeOffset,
    const QVector4D& sourceCoordTransform, const QVector4D& sourceMaskTransform,
    const quint32 backgroundColor[3])
{
    // Used by subclasses, using the shapeTextureInfo.offset constant directly allows slightly
    // better optimization here.
    Q_UNUSED(shapeOffset);

    ShapeNode::Vertex* v = reinterpret_cast<ShapeNode::Vertex*>(
        static_cast<ShapeNode*>(node)->geometry()->vertexData());

    // Set top row of 3 vertices.
    v[0].position[0] = 0.0f;
    v[0].position[1] = 0.0f;
    v[0].shapeCoordinate[0] = shapeTextureInfo.offset;
    v[0].shapeCoordinate[1] = shapeTextureInfo.offset;
    v[0].sourceCoordinate[0] = sourceCoordTransform.z();
    v[0].sourceCoordinate[1] = sourceCoordTransform.w();
    v[0].sourceCoordinate[2] = sourceMaskTransform.z();
    v[0].sourceCoordinate[3] = sourceMaskTransform.w();
    v[0].backgroundColor = backgroundColor[0];
    v[1].position[0] = 0.5f * itemSize.width();
    v[1].position[1] = 0.0f;
    v[1].shapeCoordinate[0] = (0.5f * itemSize.width()) / radius - shapeTextureInfo.offset;
    v[1].shapeCoordinate[1] = shapeTextureInfo.offset;
    v[1].sourceCoordinate[0] = 0.5f * sourceCoordTransform.x() + sourceCoordTransform.z();
    v[1].sourceCoordinate[1] = sourceCoordTransform.w();
    v[1].sourceCoordinate[2] = 0.5f * sourceMaskTransform.x() + sourceMaskTransform.z();
    v[1].sourceCoordinate[3] = sourceMaskTransform.w();
    v[1].backgroundColor = backgroundColor[0];
    v[2].position[0] = itemSize.width();
    v[2].position[1] = 0.0f;
    v[2].shapeCoordinate[0] = shapeTextureInfo.offset;
    v[2].shapeCoordinate[1] = shapeTextureInfo.offset;
    v[2].sourceCoordinate[0] = sourceCoordTransform.x() + sourceCoordTransform.z();
    v[2].sourceCoordinate[1] = sourceCoordTransform.w();
    v[2].sourceCoordinate[2] = sourceMaskTransform.x() + sourceMaskTransform.z();
    v[2].sourceCoordinate[3] = sourceMaskTransform.w();
    v[2].backgroundColor = backgroundColor[0];

    // Set middle row of 3 vertices.
    v[3].position[0] = 0.0f;
    v[3].position[1] = 0.5f * itemSize.height();
    v[3].shapeCoordinate[0] = shapeTextureInfo.offset;
    v[3].shapeCoordinate[1] = (0.5f * itemSize.height()) / radius - shapeTextureInfo.offset;
    v[3].sourceCoordinate[0] = sourceCoordTransform.z();
    v[3].sourceCoordinate[1] = 0.5f * sourceCoordTransform.y() + sourceCoordTransform.w();
    v[3].sourceCoordinate[2] = sourceMaskTransform.z();
    v[3].sourceCoordinate[3] = 0.5f * sourceMaskTransform.y() + sourceMaskTransform.w();
    v[3].backgroundColor = backgroundColor[1];
    v[4].position[0] = 0.5f * itemSize.width();
    v[4].position[1] = 0.5f * itemSize.height();
    v[4].shapeCoordinate[0] = (0.5f * itemSize.width()) / radius - shapeTextureInfo.offset;
    v[4].shapeCoordinate[1] = (0.5f * itemSize.height()) / radius - shapeTextureInfo.offset;
    v[4].sourceCoordinate[0] = 0.5f * sourceCoordTransform.x() + sourceCoordTransform.z();
    v[4].sourceCoordinate[1] = 0.5f * sourceCoordTransform.y() + sourceCoordTransform.w();
    v[4].sourceCoordinate[2] = 0.5f * sourceMaskTransform.x() + sourceMaskTransform.z();
    v[4].sourceCoordinate[3] = 0.5f * sourceMaskTransform.y() + sourceMaskTransform.w();
    v[4].backgroundColor = backgroundColor[1];
    v[5].position[0] = itemSize.width();
    v[5].position[1] = 0.5f * itemSize.height();
    v[5].shapeCoordinate[0] = shapeTextureInfo.offset;
    v[5].shapeCoordinate[1] = (0.5f * itemSize.height()) / radius - shapeTextureInfo.offset;
    v[5].sourceCoordinate[0] = sourceCoordTransform.x() + sourceCoordTransform.z();
    v[5].sourceCoordinate[1] = 0.5f * sourceCoordTransform.y() + sourceCoordTransform.w();
    v[5].sourceCoordinate[2] = sourceMaskTransform.x() + sourceMaskTransform.z();
    v[5].sourceCoordinate[3] = 0.5f * sourceMaskTransform.y() + sourceMaskTransform.w();
    v[5].backgroundColor = backgroundColor[1];

    // Set bottom row of 3 vertices.
    v[6].position[0] = 0.0f;
    v[6].position[1] = itemSize.height();
    v[6].shapeCoordinate[0] = shapeTextureInfo.offset;
    v[6].shapeCoordinate[1] = shapeTextureInfo.offset;
    v[6].sourceCoordinate[0] = sourceCoordTransform.z();
    v[6].sourceCoordinate[1] = sourceCoordTransform.y() + sourceCoordTransform.w();
    v[6].sourceCoordinate[2] = sourceMaskTransform.z();
    v[6].sourceCoordinate[3] = sourceMaskTransform.y() + sourceMaskTransform.w();
    v[6].backgroundColor = backgroundColor[2];
    v[7].position[0] = 0.5f * itemSize.width();
    v[7].position[1] = itemSize.height();
    v[7].shapeCoordinate[0] = (0.5f * itemSize.width()) / radius - shapeTextureInfo.offset;
    v[7].shapeCoordinate[1] = shapeTextureInfo.offset;
    v[7].sourceCoordinate[0] = 0.5f * sourceCoordTransform.x() + sourceCoordTransform.z();
    v[7].sourceCoordinate[1] = sourceCoordTransform.y() + sourceCoordTransform.w();
    v[7].sourceCoordinate[2] = 0.5f * sourceMaskTransform.x() + sourceMaskTransform.z();
    v[7].sourceCoordinate[3] = sourceMaskTransform.y() + sourceMaskTransform.w();
    v[7].backgroundColor = backgroundColor[2];
    v[8].position[0] = itemSize.width();
    v[8].position[1] = itemSize.height();
    v[8].shapeCoordinate[0] = shapeTextureInfo.offset;
    v[8].shapeCoordinate[1] = shapeTextureInfo.offset;
    v[8].sourceCoordinate[0] = sourceCoordTransform.x() + sourceCoordTransform.z();
    v[8].sourceCoordinate[1] = sourceCoordTransform.y() + sourceCoordTransform.w();
    v[8].sourceCoordinate[2] = sourceMaskTransform.x() + sourceMaskTransform.z();
    v[8].sourceCoordinate[3] = sourceMaskTransform.y() + sourceMaskTransform.w();
    v[8].backgroundColor = backgroundColor[2];

    node->markDirty(QSGNode::DirtyGeometry);
}
