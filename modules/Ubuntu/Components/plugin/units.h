#ifndef UBUNTU_COMPONENTS_UNITS_H
#define UBUNTU_COMPONENTS_UNITS_H

#include <QObject>
#include <QtCore/QHash>

struct Bucket {
    QString name;
    int density;
    float scaleFactor;
};

class Units : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float scaleFactor READ scaleFactor NOTIFY scaleFactorChanged)
    Q_PROPERTY(QString bucket READ bucket WRITE setBucket NOTIFY bucketChanged)

public:
    explicit Units(QObject *parent = 0);
    Q_INVOKABLE float dp(float value);

    // getters
    float scaleFactor();
    QString bucket();

    // setters
    void setBucket(QString bucketName);

Q_SIGNALS:
    void scaleFactorChanged();
    void bucketChanged();

protected:
    QString selectBucket(float density, QString formFactor);

private:
    QHash<QString, Bucket> m_densityBuckets;
    Bucket m_bucket;
};

class QQmlContext;

class UnitsChangeListener : public QObject
{
    Q_OBJECT
public:
    explicit UnitsChangeListener(QQmlContext* context);
    Q_SLOT void updateUnitsInstance();
    QQmlContext* m_context;
};

#endif // UBUNTU_COMPONENTS_UNITS_H
