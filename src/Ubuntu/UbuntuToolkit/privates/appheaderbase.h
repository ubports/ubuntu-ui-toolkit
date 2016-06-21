/*
 * Copyright 2016 Canonical Ltd.
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

#ifndef APPHEADERBASE_H
#define APPHEADERBASE_H

#include "ucheader.h"

UT_NAMESPACE_BEGIN

class UCTheme;
class UBUNTUTOOLKIT_EXPORT UCAppHeaderBase : public UCHeader
{
    Q_OBJECT
    Q_PROPERTY(bool animate MEMBER m_animate WRITE setAnimate NOTIFY animateChanged FINAL)

    // FIXME Re-expose properties that would be inaccessible due to a QML bug
    // https://bugs.launchpad.net/ubuntu/+source/qtdeclarative-opensource-src/+bug/1389721
#ifdef Q_QDOC
    Q_PROPERTY(UCTheme *theme
            READ theme2
            WRITE setTheme2
            NOTIFY themeChanged2 FINAL)
#else
    Q_PROPERTY(UT_PREPEND_NAMESPACE(UCTheme) *theme
            READ theme2
            WRITE setTheme2
            NOTIFY themeChanged2 FINAL)
#endif

public:
    explicit UCAppHeaderBase(QQuickItem *parent = 0);
    void setAnimate(bool animate);

    UCTheme* theme2();
    void setTheme2(UCTheme* value);

Q_SIGNALS:
    void animateChanged();
    void themeChanged2();

protected:
    void show(bool animate) override;
    void hide(bool animate) override;

private:
    bool m_animate:1;
};

UT_NAMESPACE_END

#endif // APPHEADERBASE_H
