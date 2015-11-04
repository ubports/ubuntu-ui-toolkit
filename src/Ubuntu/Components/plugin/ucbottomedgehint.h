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
 * Authors: Zsombor Egri <zsombor.egri@canonical.com>
 */

#ifndef UCBOTTOMEDGEHINT_H
#define UCBOTTOMEDGEHINT_H

#include "ucstyleditembase.h"

class QQuickFlickable;
class UCBottomEdgeHint : public UCStyledItemBase
{
    Q_OBJECT
    Q_ENUMS(State)
    Q_PROPERTY(QString text MEMBER m_text NOTIFY textChanged FINAL)
    Q_PROPERTY(QUrl iconSource MEMBER m_iconSource NOTIFY iconSourceChanged FINAL)
    Q_PROPERTY(QString iconName MEMBER m_iconName NOTIFY iconNameChanged FINAL)
    Q_PROPERTY(QQuickFlickable *flickable MEMBER m_flickable NOTIFY flickableChanged FINAL)
    Q_PROPERTY(State state MEMBER m_state NOTIFY stateChanged FINAL)
public:
    enum State {
        Hidden,
        Idle,
        Active,
        Locked,
        // backward compatibility, should be removed from the final
        Visible = Active, // backward compatibility
    };

    UCBottomEdgeHint(QQuickItem *parent = 0);

Q_SIGNALS:
    void textChanged();
    void iconSourceChanged();
    void iconNameChanged();
    void flickableChanged();
    void stateChanged();

    void clicked();

protected:
    void itemChange(ItemChange change, const ItemChangeData &data);

private:
    QString m_text;
    QUrl m_iconSource;
    QString m_iconName;
    QQuickFlickable *m_flickable;
    State m_state;
};

#endif // UCBOTTOMEDGEHINT_H
