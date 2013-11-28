QT += quick-private qml-private core-private

equals(QT_MAJOR_VERSION, 5):lessThan(QT_MINOR_VERSION, 2) {
    QT += v8-private
}

