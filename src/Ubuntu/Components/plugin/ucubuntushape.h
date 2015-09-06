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

#ifndef UCUBUNTUSHAPE_H
#define UCUBUNTUSHAPE_H

#include <QtQuick/QQuickItem>
#include <QtQuick/QSGNode>
#include <QtQuick/qsgtexture.h>
#include <QtQuick/qsgmaterial.h>
#include <QtGui/QOpenGLFunctions>

class UCUbuntuShape;

// --- Scene graph shader ---

class ShapeShader : public QSGMaterialShader
{
public:
    ShapeShader();
    virtual char const* const* attributeNames() const;
    virtual void initialize();
    virtual void updateState(
        const RenderState& state, QSGMaterial* newEffect, QSGMaterial* oldEffect);

private:
    QOpenGLFunctions* m_functions;
    int m_matrixId;
    int m_opacityFactorsId;
    int m_dfdtFactorId;
    int m_sourceOpacityId;
    int m_distanceAAId;
    int m_texturedId;
    int m_aspectId;
};

// --- Scene graph material ---

class ShapeMaterial : public QSGMaterial
{
public:
    struct Data {
        enum {
            Textured             = (1 << 0),
            HorizontallyRepeated = (1 << 1),
            VerticallyRepeated   = (1 << 2),
            Repeated             = (HorizontallyRepeated | VerticallyRepeated),
            Flat                 = (1 << 3),
            Inset                = (1 << 4),
            DropShadow           = (1 << 5),
            AspectMask           = (Flat | Inset | DropShadow),
            Pressed              = (1 << 6)
        };
        QSGTextureProvider* sourceTextureProvider;
        quint32 shapeTextureId;
        quint8 distanceAAFactor;
        quint8 sourceOpacity;
        quint8 flags;
    };

    ShapeMaterial();
    virtual QSGMaterialType* type() const;
    virtual QSGMaterialShader* createShader() const;
    virtual int compare(const QSGMaterial* other) const;
    virtual void updateTextures();
    const Data* constData() const { return &m_data; }
    Data* data() { return &m_data; }

private:
    Data m_data;
};

// --- Scene graph node ---

class ShapeNode : public QSGGeometryNode
{
public:
    struct Vertex {
        float position[2];
        float shapeCoordinate[2];
        float sourceCoordinate[4];
        quint32 backgroundColor;
    };

    static const int indexCount = 14;
    static const int indexType = GL_UNSIGNED_SHORT;
    static const int indexTypeSize = sizeof(unsigned short);
    static const int vertexCount = 9;
    static const QSGGeometry::DataPattern indexDataPattern = QSGGeometry::StaticPattern;
    static const QSGGeometry::DataPattern vertexDataPattern = QSGGeometry::AlwaysUploadPattern;
    static const GLenum drawingMode = GL_TRIANGLE_STRIP;
    static const unsigned short* indices();
    static const QSGGeometry::AttributeSet& attributeSet();

    ShapeNode();
    ShapeMaterial* material() { return &m_material; }
    QSGGeometry* geometry() { return &m_geometry; }
    void preprocess() { m_material.updateTextures(); }

private:
    ShapeMaterial m_material;
    QSGGeometry m_geometry;
};

// --- QtQuick item ---

class UCUbuntuShape : public QQuickItem
{
    Q_OBJECT

    // Shape properties.
    Q_ENUMS(Aspect)
    Q_PROPERTY(QString radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(Aspect aspect READ aspect WRITE setAspect NOTIFY aspectChanged REVISION 1)
    Q_PROPERTY(qreal relativeRadius READ relativeRadius WRITE setRelativeRadius
               NOTIFY relativeRadiusChanged REVISION 2)

    // Source properties.
    Q_ENUMS(FillMode)
    Q_ENUMS(WrapMode)
    Q_ENUMS(HAlignment)
    Q_ENUMS(VAlignment)
    Q_PROPERTY(QVariant source READ source WRITE setSource NOTIFY sourceChanged REVISION 1)
    Q_PROPERTY(qreal sourceOpacity READ sourceOpacity WRITE setSourceOpacity
               NOTIFY sourceOpacityChanged REVISION 1)
    Q_PROPERTY(FillMode sourceFillMode READ sourceFillMode WRITE setSourceFillMode
               NOTIFY sourceFillModeChanged REVISION 1)
    Q_PROPERTY(WrapMode sourceHorizontalWrapMode READ sourceHorizontalWrapMode
               WRITE setSourceHorizontalWrapMode NOTIFY sourceHorizontalWrapModeChanged REVISION 1)
    Q_PROPERTY(WrapMode sourceVerticalWrapMode READ sourceVerticalWrapMode
               WRITE setSourceVerticalWrapMode NOTIFY sourceVerticalWrapModeChanged REVISION 1)
    Q_PROPERTY(HAlignment sourceHorizontalAlignment READ sourceHorizontalAlignment
               WRITE setSourceHorizontalAlignment NOTIFY sourceHorizontalAlignmentChanged
               REVISION 1)
    Q_PROPERTY(VAlignment sourceVerticalAlignment READ sourceVerticalAlignment
               WRITE setSourceVerticalAlignment NOTIFY sourceVerticalAlignmentChanged REVISION 1)
    Q_PROPERTY(QVector2D sourceTranslation READ sourceTranslation WRITE setSourceTranslation
               NOTIFY sourceTranslationChanged REVISION 1)
    Q_PROPERTY(QVector2D sourceScale READ sourceScale WRITE setSourceScale
               NOTIFY sourceScaleChanged REVISION 1)

    // Background properties.
    Q_ENUMS(BackgroundMode)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor
               NOTIFY backgroundColorChanged REVISION 1)
    Q_PROPERTY(QColor secondaryBackgroundColor READ secondaryBackgroundColor
               WRITE setSecondaryBackgroundColor NOTIFY secondaryBackgroundColorChanged REVISION 1)
    Q_PROPERTY(BackgroundMode backgroundMode READ backgroundMode WRITE setBackgroundMode
               NOTIFY backgroundModeChanged REVISION 1)

    // Deprecated properties.
    Q_PROPERTY(QString borderSource READ borderSource WRITE setBorderSource
               NOTIFY borderSourceChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QColor gradientColor READ gradientColor WRITE setGradientColor
               NOTIFY gradientColorChanged)
    Q_PROPERTY(QVariant image READ image WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY(bool stretched READ stretched WRITE setStretched NOTIFY stretchedChanged)
    Q_PROPERTY(HAlignment horizontalAlignment READ horizontalAlignment WRITE setHorizontalAlignment
               NOTIFY horizontalAlignmentChanged)
    Q_PROPERTY(VAlignment verticalAlignment READ verticalAlignment WRITE setVerticalAlignment
               NOTIFY verticalAlignmentChanged)

public:
    UCUbuntuShape(QQuickItem* parent=0);

    enum Aspect { Flat = 0, Inset = 1, DropShadow = 2 };  // Don't forget to update private enum.
    enum BackgroundMode { SolidColor = 0, VerticalGradient = 1 };
    enum HAlignment { AlignLeft = 0, AlignHCenter = 1, AlignRight = 2 };
    enum VAlignment { AlignTop = 0, AlignVCenter = 1, AlignBottom = 2 };
    enum FillMode { Stretch = 0, PreserveAspectFit = 1, PreserveAspectCrop = 2, Pad = 3 };
    enum WrapMode { Transparent = 0, Repeat = 1 };

    QString radius() const {
      return (m_radius == Small) ? "small" : ((m_radius == Medium) ? "medium" : "large"); }
    void setRadius(const QString& radius);
    Aspect aspect() const { return (m_flags & AspectSet) ? static_cast<Aspect>(m_aspect) : Flat; }
    void setAspect(Aspect aspect);
    qreal relativeRadius() const { return m_relativeRadius * 0.01; }
    void setRelativeRadius(qreal relativeRadius);

    QVariant source() const {
        return QVariant::fromValue((m_flags & SourceApiSet) ? m_source : NULL); }
    void setSource(const QVariant& source);
    qreal sourceOpacity() const { return m_sourceOpacity / static_cast<qreal>(0xff); }
    void setSourceOpacity(qreal sourceOpacity);
    FillMode sourceFillMode() const { return m_sourceFillMode; }
    void setSourceFillMode(FillMode sourceFillMode);
    WrapMode sourceHorizontalWrapMode() const { return m_sourceHorizontalWrapMode; }
    void setSourceHorizontalWrapMode(WrapMode sourceHorizontalWrapMode);
    WrapMode sourceVerticalWrapMode() const { return m_sourceVerticalWrapMode; }
    void setSourceVerticalWrapMode(WrapMode sourceVerticalWrapMode);
    HAlignment sourceHorizontalAlignment() const { return m_sourceHorizontalAlignment; }
    void setSourceHorizontalAlignment(HAlignment sourceHorizontalAlignment);
    VAlignment sourceVerticalAlignment() const { return m_sourceVerticalAlignment; }
    void setSourceVerticalAlignment(VAlignment sourceVerticalAlignment);
    QVector2D sourceTranslation() const { return m_sourceTranslation; }
    void setSourceTranslation(const QVector2D& sourceTranslation);
    QVector2D sourceScale() const { return m_sourceScale; }
    void setSourceScale(const QVector2D& sourceScale);
    QColor backgroundColor() const {
        return (m_flags & BackgroundApiSet) ?
            QColor(qRed(m_backgroundColor), qGreen(m_backgroundColor), qBlue(m_backgroundColor),
                   qAlpha(m_backgroundColor)) :
            QColor(0, 0, 0, 0); }
    void setBackgroundColor(const QColor& backgroundColor);
    QColor secondaryBackgroundColor() const {
        return (m_flags & BackgroundApiSet) ?
            QColor(qRed(m_secondaryBackgroundColor), qGreen(m_secondaryBackgroundColor),
                   qBlue(m_secondaryBackgroundColor), qAlpha(m_secondaryBackgroundColor)) :
            QColor(0, 0, 0, 0); }
    void setSecondaryBackgroundColor(const QColor& secondaryBackgroundColor);
    BackgroundMode backgroundMode() const { return m_backgroundMode; }
    void setBackgroundMode(BackgroundMode backgroundMode);

    QString borderSource() const {
        return (m_flags & AspectSet) ? "" :
            ((m_aspect == Flat) ? "" :
             ((m_aspect == Inset) ? "radius_idle.sci" : "radius_pressed.sci")); }
    void setBorderSource(const QString& borderSource);
    QColor color() const {
        return (m_flags & BackgroundApiSet) ?
            QColor(0, 0, 0, 0) :
            QColor(qRed(m_backgroundColor), qGreen(m_backgroundColor), qBlue(m_backgroundColor),
                   qAlpha(m_backgroundColor)); }
    void setColor(const QColor& color);
    QColor gradientColor() const {
        return (m_flags & BackgroundApiSet) ?
            QColor(0, 0, 0, 0) :
            QColor(qRed(m_secondaryBackgroundColor), qGreen(m_secondaryBackgroundColor),
                   qBlue(m_secondaryBackgroundColor), qAlpha(m_secondaryBackgroundColor)); }
    void setGradientColor(const QColor& gradientColor);
    QVariant image() const {
        return QVariant::fromValue((m_flags & SourceApiSet) ? NULL : m_source); }
    void setImage(const QVariant& image);
    bool stretched() const { return !!(m_flags & Stretched); }
    void setStretched(bool stretched);
    HAlignment horizontalAlignment() const { return m_imageHorizontalAlignment; }
    void setHorizontalAlignment(HAlignment horizontalAlignment);
    VAlignment verticalAlignment() const { return m_imageVerticalAlignment; }
    void setVerticalAlignment(VAlignment verticalAlignment);

Q_SIGNALS:
    void radiusChanged();
    Q_REVISION(1) void aspectChanged();
    Q_REVISION(2) void relativeRadiusChanged();

    Q_REVISION(1) void sourceChanged();
    Q_REVISION(1) void sourceOpacityChanged();
    Q_REVISION(1) void sourceFillModeChanged();
    Q_REVISION(1) void sourceHorizontalWrapModeChanged();
    Q_REVISION(1) void sourceVerticalWrapModeChanged();
    Q_REVISION(1) void sourceHorizontalAlignmentChanged();
    Q_REVISION(1) void sourceVerticalAlignmentChanged();
    Q_REVISION(1) void sourceTranslationChanged();
    Q_REVISION(1) void sourceScaleChanged();

    Q_REVISION(1) void backgroundColorChanged();
    Q_REVISION(1) void secondaryBackgroundColorChanged();
    Q_REVISION(1) void backgroundModeChanged();

    void borderSourceChanged();
    void colorChanged();
    void gradientColorChanged();
    void imageChanged();
    void stretchedChanged();
    void horizontalAlignmentChanged();
    void verticalAlignmentChanged();

protected:
    virtual QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data);
    virtual void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry);

    // Virtual functions for extended shapes.
    virtual QSGNode* createSceneGraphNode() const;
    virtual void updateMaterial(QSGNode* node, float radius, quint32 shapeTextureId, bool textured);
    virtual void updateGeometry(
        QSGNode* node, const QSizeF& itemSize, float radius, float shapeOffset,
        const QVector4D& sourceCoordTransform, const QVector4D& sourceMaskTransform,
        const quint32 backgroundColor[3]);

private Q_SLOTS:
    void _q_imagePropertiesChanged();
    void _q_gridUnitChanged();
    void _q_providerDestroyed(QObject* object=0);
    void _q_textureChanged();

private:
    void updateFromImageProperties(QQuickItem* image);
    void connectToPropertyChange(
        QObject* sender, const char* property, QObject* receiver, const char* slot);
    void connectToImageProperties(QQuickItem* image);
    void dropColorSupport();
    void dropImageSupport();
    void updateSourceTransform(
        float itemWidth, float itemHeight, FillMode fillMode, HAlignment horizontalAlignment,
        VAlignment verticalAlignment, const QSize& textureSize);

    enum Radius { Small = 0, Medium = 1, Large = 2 };
    enum { Pressed = 3 };  // Aspect extension (to keep support for deprecated aspects).
    enum {
        AspectSet            = (1 << 0),
        GradientColorSet     = (1 << 1),
        BackgroundApiSet     = (1 << 2),
        SourceApiSet         = (1 << 3),
        Stretched            = (1 << 4),
        DirtySourceTransform = (1 << 5)
    };

    QQuickItem* m_source;
    QSGTextureProvider* m_sourceTextureProvider;
    QRgb m_backgroundColor;
    QRgb m_secondaryBackgroundColor;
    QVector2D m_sourceScale;
    QVector2D m_sourceTranslation;
    QVector4D m_sourceTransform;
    quint8 m_relativeRadius;
    Radius m_radius : 2;
    quint8 m_aspect : 3;
    HAlignment m_imageHorizontalAlignment : 2;
    VAlignment m_imageVerticalAlignment : 2;
    BackgroundMode m_backgroundMode : 1;
    HAlignment m_sourceHorizontalAlignment : 2;
    VAlignment m_sourceVerticalAlignment : 2;
    FillMode m_sourceFillMode : 2;
    WrapMode m_sourceHorizontalWrapMode : 1;
    WrapMode m_sourceVerticalWrapMode : 1;
    quint8 __explicit_padding : 6;
    quint8 m_sourceOpacity;
    quint8 m_flags;

    Q_DISABLE_COPY(UCUbuntuShape)
};

QML_DECLARE_TYPE(UCUbuntuShape)

#endif  // UCUBUNTUSHAPE_H
