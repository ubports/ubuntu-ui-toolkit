# Do not use CONFIG += testcase that would add a 'make check' because it also
# adds a 'make install' that installs the test cases, which we do not want.
# Instead add a 'make check' manually.

check.target = check

DISPLAY = $$system(export|grep DISPLAY)

!isEmpty(DISPLAY) {
 check.commands = "set -e;"
 for(TEST, TESTS) {
  check.commands += ../../unit/runtest.sh $${TARGET} $${TEST};
 }
}

isEmpty(DISPLAY) {
  message("DISPLAY is not set. Skip tests.");
}
