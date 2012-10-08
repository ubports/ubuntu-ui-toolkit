#ifndef UBUNTU_COMPONENTS_UNITS_H
#define UBUNTU_COMPONENTS_UNITS_H

#include <QObject>
#include <QtCore/QHash>
#include <QtCore/QUrl>

struct Bucket {
    QString suffix;
    int density;
    float scaleFactor;
};

class Units : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float scaleFactor READ scaleFactor WRITE setScaleFactor NOTIFY scaleFactorChanged)

public:
    static Units& instance() {
        static Units instance;
        return instance;
    }

    explicit Units(QObject *parent = 0);
    Q_INVOKABLE float dp(float value);
    Q_INVOKABLE QString resolveResource(const QUrl& value);

    // getters
    float scaleFactor();

    // setters
    void setScaleFactor(float scaleFactor);

Q_SIGNALS:
    void scaleFactorChanged();

private:
    float m_scaleFactor;
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
