include(../test-include.pri)
SOURCES += tst_components_benchmark.cpp

check.commands = ./$$TARGET -platform minimal -maxwarnings 1 -xunitxml >
check.commands += ../../$(TARGET).xml
