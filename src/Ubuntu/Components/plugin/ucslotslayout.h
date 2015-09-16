/*
 * Copyright 2015 Canonical Ltd.
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
 * Author: Andrea Bernabei <andrea.bernabei@canonical.com>
 */

#ifndef UCSLOTSLAYOUT_H
#define UCSLOTSLAYOUT_H

#include <QtQuick/QQuickItem>
#include <QtGui/QPixmap>
#include <private/qquicktext_p.h>

class UCSlotsAttached;
class UCSlotsLayoutMargins;
class UCSlotsLayoutPrivate;
class UCSlotsLayout : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QQuickText *titleItem READ titleItem CONSTANT FINAL)
    Q_PROPERTY(QQuickText *subtitleItem READ subtitleItem CONSTANT FINAL)
    Q_PROPERTY(QQuickText *subsubtitleItem READ subsubtitleItem CONSTANT FINAL)
    Q_PROPERTY(UCSlotsLayoutMargins *contentMargins READ contentMargins CONSTANT FINAL)

    Q_ENUMS(UCSlotPosition)

public:
    explicit UCSlotsLayout(QQuickItem *parent = 0);

    //this methods can't be const because otherwise they'd have to return
    //unmodifiable labels, since they're allocated on the stack, and that would
    //fail compilation (unless const_cast is used)
    QQuickText *titleItem();
    QQuickText *subtitleItem();
    QQuickText *subsubtitleItem();

    UCSlotsLayoutMargins *contentMargins();

    enum UCSlotPosition { LeadingBeginning, Leading, LeadingEnd,
                          TrailingBeginning, Trailing, TrailingEnd };

    static UCSlotsAttached *qmlAttachedProperties(QObject *object);

protected:
    Q_DECLARE_PRIVATE(UCSlotsLayout)
    void componentComplete();
    void itemChange(ItemChange change, const ItemChangeData &data);

private:
    Q_PRIVATE_SLOT(d_func(), void _q_onThemeChanged())
    Q_PRIVATE_SLOT(d_func(), void _q_onGuValueChanged())
    Q_PRIVATE_SLOT(d_func(), void _q_updateCachedHeight())
    Q_PRIVATE_SLOT(d_func(), void _q_updateGuValues())
    Q_PRIVATE_SLOT(d_func(), void _q_updateLabelsAnchorsAndBBoxHeight())
    Q_PRIVATE_SLOT(d_func(), void _q_updateSlotsBBoxHeight())
    Q_PRIVATE_SLOT(d_func(), void _q_updateSize())
    Q_PRIVATE_SLOT(d_func(), void _q_relayout())
};
QML_DECLARE_TYPEINFO(UCSlotsLayout, QML_HAS_ATTACHED_PROPERTIES)

class UCSlotsAttachedPrivate;
class UCSlotsAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(UCSlotsLayout::UCSlotPosition position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(qreal leftMargin READ leftMargin WRITE setLeftMarginQml NOTIFY leftMarginChanged)
    Q_PROPERTY(qreal rightMargin READ rightMargin WRITE setRightMarginQml NOTIFY rightMarginChanged)
    Q_PROPERTY(bool overrideVerticalPositioning READ overrideVerticalPositioning WRITE setOverrideVerticalPositioning NOTIFY overrideVerticalPositioningChanged)

public:
    UCSlotsAttached(QObject *object = 0);

    UCSlotsLayout::UCSlotPosition position() const;
    void setPosition(UCSlotsLayout::UCSlotPosition pos);

    qreal leftMargin() const;
    void setLeftMargin(qreal margin);
    void setLeftMarginQml(qreal margin);

    qreal rightMargin() const;
    void setRightMargin(qreal margin);
    void setRightMarginQml(qreal margin);

    bool overrideVerticalPositioning() const;
    void setOverrideVerticalPositioning(bool val);

Q_SIGNALS:
    void positionChanged();
    void leftMarginChanged();
    void rightMarginChanged();
    void overrideVerticalPositioningChanged();

protected:
    Q_DECLARE_PRIVATE(UCSlotsAttached)

private:
    Q_PRIVATE_SLOT(d_func(), void _q_onGuValueChanged())
};

class UCSlotsLayoutMargins : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal leftMargin READ leftMargin WRITE setLeftMarginQml NOTIFY leftMarginChanged FINAL)
    Q_PROPERTY(qreal rightMargin READ rightMargin WRITE setRightMarginQml NOTIFY rightMarginChanged FINAL)
    Q_PROPERTY(qreal topMargin READ topMargin WRITE setTopMarginQml NOTIFY topMarginChanged FINAL)
    Q_PROPERTY(qreal bottomMargin READ bottomMargin WRITE setBottomMarginQml NOTIFY bottomMarginChanged FINAL)

public:
    explicit UCSlotsLayoutMargins(QObject *parent = 0);

    qreal leftMargin() const;
    void setLeftMargin(qreal val);
    void setLeftMarginQml(qreal val);

    qreal rightMargin() const;
    void setRightMargin(qreal val);
    void setRightMarginQml(qreal val);

    qreal topMargin() const;
    void setTopMargin(qreal val);
    void setTopMarginQml(qreal val);

    qreal bottomMargin() const;
    void setBottomMargin(qreal val);
    void setBottomMarginQml(qreal val);

    //once the dev tries to change the offsets (and he does so via QML) we'll stop
    //updating offset's value, for instance when gu value changes or when the
    //positioning mode changes
    bool leftMarginWasSetFromQml : 1;
    bool rightMarginWasSetFromQml : 1;
    bool topMarginWasSetFromQml : 1;
    bool bottomMarginWasSetFromQml : 1;

Q_SIGNALS:
    void leftMarginChanged();
    void rightMarginChanged();
    void topMarginChanged();
    void bottomMarginChanged();

private:
    //similar to anchors.margins, but we don't use a contentItem so we handle this ourselves
    qreal m_leftMargin;
    qreal m_rightMargin;
    qreal m_topMargin;
    qreal m_bottomMargin;
};

#endif // UCSLOTSLAYOUT_H
