include(../test-include.pri)

CONFIG += custom_qpa   # needed by test to set device pixel ratio correctly
QT += gui
SOURCES += tst_mainwindow.cpp
