include(../test-include.pri)
SOURCES += tst_components_benchmark.cpp

check.commands = ./$$TARGET -platform minimal -maxwarnings 1; ./$$TARGET -platform minimal -maxwarnings 1 -tickcounter;
