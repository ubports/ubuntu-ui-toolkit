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
    QOpenGLFunctions* functions_;
    int matrixId_;
    int opacityId_;
    int sourceOpacityId_;
    int secondaryBackgroundColorId_;
    int texturedId_;
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
            Repeated             = (HorizontallyRepeated | VerticallyRepeated)
        };
        QSGTexture* shapeTexture;
        QSGTextureProvider* sourceTextureProvider;
        quint8 sourceOpacity;
        quint8 shapeTextureFiltering;
        quint8 flags;
    };

    ShapeMaterial();
    virtual QSGMaterialType* type() const;
    virtual QSGMaterialShader* createShader() const;
    virtual int compare(const QSGMaterial* other) const;
    const Data* constData() const { return &data_; }
    Data* data() { return &data_; }

private:
    Data data_;
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

    static const int indexCount = 28;
    static const int indexType = GL_UNSIGNED_SHORT;
    static const int indexTypeSize = sizeof(unsigned short);
    static const int vertexCount = 16;
    static const QSGGeometry::DataPattern indexDataPattern = QSGGeometry::StaticPattern;
    static const QSGGeometry::DataPattern vertexDataPattern = QSGGeometry::AlwaysUploadPattern;
    static const GLenum drawingMode = GL_TRIANGLE_STRIP;
    static const unsigned short* indices();
    static const QSGGeometry::AttributeSet& attributeSet();

    ShapeNode();
    ShapeMaterial* material() { return &material_; }
    QSGGeometry* geometry() { return &geometry_; }

private:
    QSGGeometry geometry_;
    ShapeMaterial material_;
};

// --- QtQuick item ---

class UCUbuntuShape : public QQuickItem
{
    Q_OBJECT

    // Shape properties.
    Q_PROPERTY(QString radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(QString borderSource READ borderSource WRITE setBorderSource
               NOTIFY borderSourceChanged)

    // Source properties.
    Q_ENUMS(FillMode)
    Q_ENUMS(WrapMode)
    Q_ENUMS(HAlignment)
    Q_ENUMS(VAlignment)
    Q_PROPERTY(QVariant source READ source WRITE setSource NOTIFY sourceChanged REVISION 1)
    Q_PROPERTY(float sourceOpacity READ sourceOpacity WRITE setSourceOpacity
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

    enum BackgroundMode { SolidColor = 0, VerticalGradient = 1 };
    enum HAlignment { AlignLeft = 0, AlignHCenter = 1, AlignRight = 2 };
    enum VAlignment { AlignTop = 0, AlignVCenter = 1, AlignBottom = 2 };
    enum FillMode { Stretch = 0, PreserveAspectFit = 1, PreserveAspectCrop = 2, Pad = 3 };
    enum WrapMode { Transparent = 0, Repeat = 1 };

    QString radius() const { return (radius_ == SmallRadius) ? "small" : "medium"; }
    void setRadius(const QString& radius);
    QString borderSource() const {
        return (border_ == IdleBorder) ? "radius_idle.sci" :
                ((border_ == PressedBorder) ? "radius_pressed.sci" : ""); }
    void setBorderSource(const QString& borderSource);

    QVariant source() const {
        return QVariant::fromValue((flags_ & SourceApiSet) ? source_ : NULL); }
    void setSource(const QVariant& source);
    float sourceOpacity() const { return sourceOpacity_ / static_cast<float>(0xff); }
    void setSourceOpacity(float sourceOpacity);
    FillMode sourceFillMode() const { return sourceFillMode_; }
    void setSourceFillMode(FillMode sourceFillMode);
    WrapMode sourceHorizontalWrapMode() const { return sourceHorizontalWrapMode_; }
    void setSourceHorizontalWrapMode(WrapMode sourceHorizontalWrapMode);
    WrapMode sourceVerticalWrapMode() const { return sourceVerticalWrapMode_; }
    void setSourceVerticalWrapMode(WrapMode sourceVerticalWrapMode);
    HAlignment sourceHorizontalAlignment() const { return sourceHorizontalAlignment_; }
    void setSourceHorizontalAlignment(HAlignment sourceHorizontalAlignment);
    VAlignment sourceVerticalAlignment() const { return sourceVerticalAlignment_; }
    void setSourceVerticalAlignment(VAlignment sourceVerticalAlignment);
    QVector2D sourceTranslation() const { return sourceTranslation_; }
    void setSourceTranslation(const QVector2D& sourceTranslation);
    QVector2D sourceScale() const { return sourceScale_; }
    void setSourceScale(const QVector2D& sourceScale);
    QColor backgroundColor() const {
        return (flags_ & BackgroundApiSet) ?
            QColor(qRed(backgroundColor_), qGreen(backgroundColor_), qBlue(backgroundColor_),
                   qAlpha(backgroundColor_)) :
            QColor(0, 0, 0, 0); }
    void setBackgroundColor(const QColor& backgroundColor);
    QColor secondaryBackgroundColor() const {
        return (flags_ & BackgroundApiSet) ?
            QColor(qRed(secondaryBackgroundColor_), qGreen(secondaryBackgroundColor_),
                   qBlue(secondaryBackgroundColor_), qAlpha(secondaryBackgroundColor_)) :
            QColor(0, 0, 0, 0); }
    void setSecondaryBackgroundColor(const QColor& secondaryBackgroundColor);
    BackgroundMode backgroundMode() const { return backgroundMode_; }
    void setBackgroundMode(BackgroundMode backgroundMode);

    QColor color() const {
        return (flags_ & BackgroundApiSet) ?
            QColor(0, 0, 0, 0) :
            QColor(qRed(backgroundColor_), qGreen(backgroundColor_), qBlue(backgroundColor_),
                   qAlpha(backgroundColor_)); }
    void setColor(const QColor& color);
    QColor gradientColor() const {
        return (flags_ & BackgroundApiSet) ?
            QColor(0, 0, 0, 0) :
            QColor(qRed(secondaryBackgroundColor_), qGreen(secondaryBackgroundColor_),
                   qBlue(secondaryBackgroundColor_), qAlpha(secondaryBackgroundColor_)); }
    void setGradientColor(const QColor& gradientColor);
    QVariant image() const { return QVariant::fromValue((flags_ & SourceApiSet) ? NULL : source_); }
    void setImage(const QVariant& image);
    bool stretched() const { return !!(flags_ & Stretched); }
    void setStretched(bool stretched);
    HAlignment horizontalAlignment() const { return imageHorizontalAlignment_; }
    void setHorizontalAlignment(HAlignment horizontalAlignment);
    VAlignment verticalAlignment() const { return imageVerticalAlignment_; }
    void setVerticalAlignment(VAlignment verticalAlignment);

Q_SIGNALS:
    void radiusChanged();
    void borderSourceChanged();

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
    virtual void updateMaterial(
        QSGNode* node, QSGTexture* shapeTexture, QSGTexture::Filtering shapeTextureFiltering,
        QSGTexture* sourceTexture);
    virtual void updateGeometry(
        QSGNode* node, float width, float height, float radius, const float shapeCoordinate[][2],
        const QVector4D& sourceCoordTransform, const QVector4D& sourceMaskTransform,
        const quint32 backgroundColor[4]);

private Q_SLOTS:
    void _q_imagePropertiesChanged();
    void _q_openglContextDestroyed();
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

    enum Radius { SmallRadius, MediumRadius };
    enum Border { RawBorder, IdleBorder, PressedBorder };
    enum {
        GradientColorSet     = (1 << 0),
        Stretched            = (1 << 1),
        BackgroundApiSet     = (1 << 2),
        SourceApiSet         = (1 << 3),
        DirtySourceTransform = (1 << 4)
    };

    QQuickItem* source_;
    QSGTextureProvider* sourceTextureProvider_;
    QRgb backgroundColor_;
    QRgb secondaryBackgroundColor_;
    QVector2D sourceScale_;
    QVector2D sourceTranslation_;
    QVector4D sourceTransform_;
    Radius radius_ : 1;
    Border border_ : 2;
    HAlignment imageHorizontalAlignment_ : 2;
    VAlignment imageVerticalAlignment_ : 2;
    BackgroundMode backgroundMode_ : 1;
    HAlignment sourceHorizontalAlignment_ : 2;
    VAlignment sourceVerticalAlignment_ : 2;
    FillMode sourceFillMode_ : 2;
    WrapMode sourceHorizontalWrapMode_ : 1;
    WrapMode sourceVerticalWrapMode_ : 1;
    quint8 sourceOpacity_;
    quint8 flags_;

    Q_DISABLE_COPY(UCUbuntuShape)
};

QML_DECLARE_TYPE(UCUbuntuShape)

#endif  // UCUBUNTUSHAPE_H
