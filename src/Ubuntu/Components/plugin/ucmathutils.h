#ifndef UCMATHUTILS_H
#define UCMATHUTILS_H

#include <QObject>

QT_FORWARD_DECLARE_CLASS(QQmlEngine)
QT_FORWARD_DECLARE_CLASS(QJSEngine)

class UCMathUtils : public QObject
{
    Q_OBJECT
public:
    explicit UCMathUtils(QObject *parent = 0);

    Q_INVOKABLE double clamp(double x, double min, double max);
    Q_INVOKABLE double lerp(double delta, double from, double to);
    Q_INVOKABLE double projectValue(double x, double xmin, double xmax, double ymin, double ymax);
    Q_INVOKABLE double clampAndProject(double x, double xmin, double xmax, double ymin, double ymax);

    static QObject *qmlInstanceProvider(QQmlEngine *engine, QJSEngine *scriptEngine);
};

#endif // UCMATHUTILS_H
