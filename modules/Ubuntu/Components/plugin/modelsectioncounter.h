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
 */

#ifndef MODELSECTIONCOUNTER_H
#define MODELSECTIONCOUNTER_H

#include <QtCore/QObject>
#include <QtCore/QAbstractListModel>

class QQuickVisualModel;

class ModelSectionCounter : public QObject
{
    Q_OBJECT

    Q_PROPERTY(SectionCriteria sectionCriteria READ sectionCriteria WRITE setSectionCriteria NOTIFY sectionCriteriaChanged)
    Q_PROPERTY(QString sectionProperty READ sectionProperty WRITE setSectionProperty NOTIFY sectionPropertyChanged)
    Q_PROPERTY(QQuickVisualModel* model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(unsigned int sectionCount READ sectionCount NOTIFY sectionCountChanged)

    Q_ENUMS(SectionCriteria)

public:
    explicit ModelSectionCounter(QObject *parent = 0);

    enum SectionCriteria { FullString, FirstCharacter };
    SectionCriteria sectionCriteria() const;
    void setSectionCriteria(SectionCriteria sectionCriteria);

    QString sectionProperty() const;
    void setSectionProperty(const QString &sectionProperty);

    QQuickVisualModel* model() const;
    void setModel(QQuickVisualModel* model);

    unsigned int sectionCount() const;

Q_SIGNALS:
    void sectionPropertyChanged();
    void modelChanged();
    void sectionCountChanged();
    void sectionCriteriaChanged();

protected Q_SLOTS:
    void updateSectionCount();

protected:
    QString sectionString(const QString &value);
    void watchSectionPropertyRole();

private:
    SectionCriteria m_sectionCriteria;
    QString m_sectionProperty;
    QQuickVisualModel* m_model;
    unsigned int m_sectionCount;
};

#endif // MODELSECTIONCOUNTER_H
