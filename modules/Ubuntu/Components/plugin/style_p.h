/*
 * Copyright 2012 Canonical Ltd.
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
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#ifndef STYLE_P_H
#define STYLE_P_H

#include "style.h"

class StyleRulePrivate
{
    Q_DECLARE_PUBLIC(StyleRule)

public:
    StyleRulePrivate(StyleRule *qq);
    StyleRulePrivate(StyleRule *qq, QQmlEngine *engine, const QString &selector, const QString &styleRule, const QString &delegateRule);
    ~StyleRulePrivate();

    StyleRule *q_ptr;

    QQmlComponent *style;
    QQmlComponent *delegate;
    QString selector;
    QString styleQml;
    QString delegateQml;
    bool qthmStyle;

    void createComponent(QQmlEngine *engine, const QString &rule, QQmlComponent **component);
    void completeComponent(QQmlComponent *sender);
    void _q_componentCompleted(QQmlComponent::Status);
};

#endif // STYLE_P_H
