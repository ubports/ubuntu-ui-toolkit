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
    static Units& instance() {
        static Units instance;
        return instance;
    }

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

private:
    float m_scaleFactor;
    QString m_bucket;
};

class QQmlContext;

class ContextPropertyChangeListener : public QObject
{
    Q_OBJECT
public:
    explicit ContextPropertyChangeListener(QQmlContext* context, QString contextProperty);
    Q_SLOT void updateContextProperty();
    QQmlContext* m_context;
    QString m_contextProperty;
};

#endif // UBUNTU_COMPONENTS_UNITS_H
