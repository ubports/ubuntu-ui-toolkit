# Do not use CONFIG += testcase that would add a 'make check' because it also
# adds a 'make install' that installs the test cases, which we do not want.
# Instead add a 'make check' manually.

check.target = check
check.commands = ""
for(TEST, TESTS) {
  check.commands += ./$$TARGET -input $${TEST} -platform minimal
  check.commands += -import \"../../../modules\"
  check.commands += -maxwarnings 20 -xunitxml -o ../../test_$(TARGET)_$${TEST}.xml;
  check.commands += ../testparser/testparser ../../test_$(TARGET)_$${TEST}.xml;

}
