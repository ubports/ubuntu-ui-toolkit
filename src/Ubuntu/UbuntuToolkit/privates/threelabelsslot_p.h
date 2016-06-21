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
 */

#ifndef THREELABELSSLOT_H
#define THREELABELSSLOT_H

#include <QQuickItem>
#include <private/qquickitem_p.h>
#include <ubuntutoolkitglobal.h>

//The padding between title and the string below it (i.e. subtitle, or
//summary, when subtitle is empty)
#define TITLE_SPACING_DP                       2

UT_NAMESPACE_BEGIN

class UCTheme;
class UCLabel;
class UCThreeLabelsSlotPrivate;
class UCThreeLabelsSlot : public QQuickItem
{
    Q_OBJECT
#ifdef Q_QDOC
    Q_PROPERTY(UCLabel *title READ title CONSTANT FINAL)
    Q_PROPERTY(UCLabel *subtitle READ subtitle CONSTANT FINAL)
    Q_PROPERTY(UCLabel *summary READ summary CONSTANT FINAL)
#else
    Q_PROPERTY(UT_PREPEND_NAMESPACE(UCLabel) *title READ title CONSTANT FINAL)
    Q_PROPERTY(UT_PREPEND_NAMESPACE(UCLabel) *subtitle READ subtitle CONSTANT FINAL)
    Q_PROPERTY(UT_PREPEND_NAMESPACE(UCLabel) *summary READ summary CONSTANT FINAL)
#endif

public:
    explicit UCThreeLabelsSlot(QQuickItem *parent = 0);

    UCLabel *title();
    UCLabel *subtitle();
    UCLabel *summary();

protected:
    Q_DECLARE_PRIVATE(UCThreeLabelsSlot)

private:
    Q_PRIVATE_SLOT(d_func(), void _q_onGuValueChanged())
    Q_PRIVATE_SLOT(d_func(), void _q_updateLabelsAnchorsAndBBoxHeight())

    static QColor getSubtitleColor(QQuickItem *item, UCTheme *theme);
    static QColor getSummaryColor(QQuickItem *item, UCTheme *theme);
};

class UCThreeLabelsSlotPrivate : public QQuickItemPrivate
{
    Q_DECLARE_PUBLIC(UCThreeLabelsSlot)

public:
    UCThreeLabelsSlotPrivate();

    static inline UCThreeLabelsSlotPrivate *get(UCThreeLabelsSlot *that)
    {
        Q_ASSERT(that);
        return that->d_func();
    }

    void init();
    void setTitleProperties();
    void setSubtitleProperties();
    void setSummaryProperties();

    void _q_onGuValueChanged();
    void _q_updateLabelsAnchorsAndBBoxHeight();

    UCLabel *m_title;
    UCLabel *m_subtitle;
    UCLabel *m_summary;
};

UT_NAMESPACE_END

#endif // THREELABELSSLOT_H
