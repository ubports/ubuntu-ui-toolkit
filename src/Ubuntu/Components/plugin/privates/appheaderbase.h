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
//#include <QtCore/QPointer>

//class QQuickFlickable;
//class QQuickNumberAnimation;
//class UCUbuntuAnimation;
//class PropertyChange;

class UCTheme;

class UCAppHeaderBase : public UCHeader
{
    Q_OBJECT
//    Q_PROPERTY(bool animate MEMBER m_animate WRITE setAnimate NOTIFY animateChanged FINAL)

    // FIXME Re-expose properties that would be inaccessible due to a QML bug
    // https://bugs.launchpad.net/ubuntu/+source/qtdeclarative-opensource-src/+bug/1389721
    Q_PROPERTY(UCTheme *theme
            READ theme2
            WRITE setTheme2
            NOTIFY themeChanged2 FINAL)
//    Q_PROPERTY(QString styleName
//            READ styleName2
//            WRITE setStyleName2
//            NOTIFY styleNameChanged2 FINAL )

public:
    explicit UCAppHeaderBase(QQuickItem *parent = 0);
//    AppHeaderBase(QQuickItem *parent = 0);

    //    void setAnimate(bool animate);

    UCTheme* theme2();
    void setTheme2(UCTheme* value);
//    QString styleName2();
//    void setStyleName2(QString value);

Q_SIGNALS:
//    void animateChanged();
    void themeChanged2();
//    void styleNameChanged2();


protected:
//    void show(bool animate);
//    void hide(bool animate);
    virtual void componentComplete();

private:
//    bool m_animate:1;
};

#endif // APPHEADERBASE_H
