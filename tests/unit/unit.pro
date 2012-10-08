include( ../../qtversioncheck.pri )

TEMPLATE = subdirs
SUBDIRS += ThemeEngineTest \
    ThemeEnginePrivate

themeengineprivatetest.target = themeengineprivatetest
themeengineprivatetest.commands = cd ThemeEnginePrivate; ./tst_themeengineprivatetest -platform minimal
themeengineprivatetest.depends = 
QMAKE_EXTRA_TARGETS += themeengineprivatetest

themeenginetest.target = themeenginetest
themeenginetest.commands = cd ThemeEngineTest; ./tst_themeenginetest -platform minimal
themeenginetest.depends = 
QMAKE_EXTRA_TARGETS += themeenginetest

test.target = test
test.commands = make; make themeengineprivatetest; make themeenginetest
test.depends =
QMAKE_EXTRA_TARGETS += test
