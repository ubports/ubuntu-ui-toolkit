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

#ifndef UCUBUNTUSHAPE_H
#define UCUBUNTUSHAPE_H

#include <QtQuick/QQuickItem>
#include <QtQuick/QSGNode>
#include <QtQuick/qsgtexture.h>
#include <QtGui/QOpenGLFunctions>

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
    Q_PROPERTY(QVariant source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(float sourceOpacity READ sourceOpacity WRITE setSourceOpacity
               NOTIFY sourceOpacityChanged)
    Q_PROPERTY(FillMode sourceFillMode READ sourceFillMode WRITE setSourceFillMode
               NOTIFY sourceFillModeChanged)
    Q_PROPERTY(WrapMode sourceHorizontalWrapMode READ sourceHorizontalWrapMode
               WRITE setSourceHorizontalWrapMode NOTIFY sourceHorizontalWrapModeChanged)
    Q_PROPERTY(WrapMode sourceVerticalWrapMode READ sourceVerticalWrapMode
               WRITE setSourceVerticalWrapMode NOTIFY sourceVerticalWrapModeChanged)
    Q_PROPERTY(HAlignment sourceHorizontalAlignment READ sourceHorizontalAlignment
               WRITE setSourceHorizontalAlignment NOTIFY sourceHorizontalAlignmentChanged)
    Q_PROPERTY(VAlignment sourceVerticalAlignment READ sourceVerticalAlignment
               WRITE setSourceVerticalAlignment NOTIFY sourceVerticalAlignmentChanged)
    Q_PROPERTY(QVector2D sourceTranslation READ sourceTranslation WRITE setSourceTranslation
               NOTIFY sourceTranslationChanged)
    Q_PROPERTY(QVector2D sourceScale READ sourceScale WRITE setSourceScale
               NOTIFY sourceScaleChanged)

    // Background properties.
    Q_ENUMS(BackgroundMode)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor
               NOTIFY backgroundColorChanged)
    Q_PROPERTY(QColor secondaryBackgroundColor READ secondaryBackgroundColor
               WRITE setSecondaryBackgroundColor NOTIFY secondaryBackgroundColorChanged)
    Q_PROPERTY(BackgroundMode backgroundMode READ backgroundMode WRITE setBackgroundMode
               NOTIFY backgroundModeChanged)

    // Overlay properties.
    Q_PROPERTY(QRectF overlayGeometry READ overlayGeometry WRITE setOverlayGeometry
               NOTIFY overlayGeometryChanged)
    Q_PROPERTY(QColor overlayColor READ overlayColor WRITE setOverlayColor
               NOTIFY overlayColorChanged)

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

    QString radius() const { return radiusString_; }
    void setRadius(const QString& radius);
    QString borderSource() const { return borderSource_; }
    void setBorderSource(const QString& borderSource);

    QVariant source() const { return QVariant::fromValue(source_); }
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

    QColor backgroundColor() const { return backgroundColor_; }
    void setBackgroundColor(const QColor& backgroundColor);
    QColor secondaryBackgroundColor() const { return secondaryBackgroundColor_; }
    void setSecondaryBackgroundColor(const QColor& secondaryBackgroundColor);
    BackgroundMode backgroundMode() const { return backgroundMode_; }
    void setBackgroundMode(BackgroundMode backgroundMode);

    QRectF overlayGeometry() const {
        const float u16ToF32 = 1.0f / static_cast<float>(0xffff);
        return QRectF(overlayX_ * u16ToF32, overlayY_ * u16ToF32, overlayWidth_ * u16ToF32,
                      overlayHeight_ * u16ToF32); }
    void setOverlayGeometry(const QRectF& overlayGeometry);
    QColor overlayColor() const { return overlayColor_; }
    void setOverlayColor(const QColor& overlayColor);

    QColor color() const { return color_; }
    void setColor(const QColor& color);
    QColor gradientColor() const { return gradientColor_; }
    void setGradientColor(const QColor& gradientColor);
    QVariant image() const { return QVariant::fromValue(image_); }
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

    void sourceChanged();
    void sourceOpacityChanged();
    void sourceFillModeChanged();
    void sourceHorizontalWrapModeChanged();
    void sourceVerticalWrapModeChanged();
    void sourceHorizontalAlignmentChanged();
    void sourceVerticalAlignmentChanged();
    void sourceTranslationChanged();
    void sourceScaleChanged();

    void backgroundColorChanged();
    void secondaryBackgroundColorChanged();
    void backgroundModeChanged();

    void overlayGeometryChanged();
    void overlayColorChanged();

    void colorChanged();
    void gradientColorChanged();
    void imageChanged();
    void stretchedChanged();
    void horizontalAlignmentChanged();
    void verticalAlignmentChanged();

protected:
    virtual QSGNode* updatePaintNode(QSGNode*, UpdatePaintNodeData*);
    virtual void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry);

private Q_SLOTS:
    void onImagePropertiesChanged();
    void onOpenglContextDestroyed();
    void gridUnitChanged();
    void providerDestroyed(QObject* object=0);
    void textureChanged();

private:
    void updateFromImageProperties(QQuickItem* image);
    void connectToPropertyChange(QObject* sender, const char* property,
                                 QObject* receiver, const char* slot);
    void connectToImageProperties(QQuickItem* image);
    void dropImageSupport();
    void updateSourceTransform(float itemWidth, float itemHeight, FillMode fillMode,
                               HAlignment horizontalAlignment, VAlignment verticalAlignment,
                               const QSize& textureSize);

    enum Radius { SmallRadius, MediumRadius };
    enum Border { RawBorder, IdleBorder, PressedBorder };
    enum {
        GradientColorSet     = (1 << 0),
        Stretched            = (1 << 1),
        BackgroundApiSet     = (1 << 2),
        SourceApiSet         = (1 << 3),
        DirtySourceTransform = (1 << 4)
    };

    QQuickItem* image_;
    QQuickItem* source_;
    QSGTextureProvider* sourceTextureProvider_;
    QRgb color_;
    QRgb gradientColor_;
    QRgb backgroundColor_;
    QRgb secondaryBackgroundColor_;
    QString radiusString_;
    QString borderSource_;
    float gridUnit_;
    QVector2D sourceScale_;
    QVector2D sourceTranslation_;
    QVector4D sourceTransform_;
    quint16 overlayX_;
    quint16 overlayY_;
    quint16 overlayWidth_;
    quint16 overlayHeight_;
    QRgb overlayColor_;
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

#endif // UCUBUNTUSHAPE_H
