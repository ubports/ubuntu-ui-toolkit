# Do not use CONFIG += testcase that would add a 'make check' because it also
# adds a 'make install' that installs the test cases, which we do not want.
# Instead add a 'make check' manually.

check.target = check
check.commands = "set -e;"
for(TEST, TESTS) {
  check.commands += ../runtest.sh $${TARGET} $${TEST};
}
check.commands += cd ../../..;
check.commands += qmlplugindump Ubuntu.Components 0.1 modules/Ubuntu/ > plugins.qmltypes;
check.commands += tests/qmlapicheck.py modules/Ubuntu/Components/*.qml plugins.qmltypes > components.api.new;
check.commands += diff -Fqml -u components.api components.api.new || exit 1; cd tests/unit
