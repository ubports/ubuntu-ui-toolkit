#ifndef UCGRAPHMODEL_H
#define UCGRAPHMODEL_H

#include <QtCore/QObject>
#include <QtGui/QImage>

class UCGraphModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QImage image READ image NOTIFY imageChanged)
    Q_PROPERTY(int shift READ shift NOTIFY shiftChanged)
    Q_PROPERTY(int samples READ samples WRITE setSamples NOTIFY samplesChanged)

public:
    explicit UCGraphModel(QObject *parent = 0);

    void appendValue(int width, int value);

    // getters
    QImage image() const;
    int shift() const;
    int samples() const;

    // setters
    void setSamples(int samples);

Q_SIGNALS:
    void imageChanged();
    void shiftChanged();
    void samplesChanged();

private:
    QImage m_image;
    int m_shift;
    int m_samples;
};

#endif // UCGRAPHMODEL_H
