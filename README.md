# Ubuntu UI Toolkit


## Introduction

This project consists of a set of QML components to ease the creation of
beautiful applications in QML for Ubuntu.

QML alone lacks built-in components for basic widgets like Button, Slider,
Scrollbar, etc, meaning a developer has to build them from scratch. This 
toolkit aims to stop this duplication of work, supplying beautiful components 
ready-made and with a clear and consistent API.

These components are fully themeable so the look and feel can be easily 
customized. Resolution independence technology is built in so UIs are scaled 
to best suit the display.

Other features:
- localisation through gettext

This project supports QML2 and above.


## Building & Setup

You need Qt5 installed to proceed. To build this repository, run
```
mkdir build
cd build
qmake ..
make -j4
```

View the components gallery by running
```
./gallery.sh
```

To install these components, run
```
make install
```
You may need to be root for this command to succeed.

To use these components in QML without installing them, you need to set the 
QML2_IMPORT_PATH environment variable. This is done by running
```
source export_qml_dir.sh
```
from the root directory of this repository, but is lost when you close your 
shell.


## Tests

tests
 - autopilot/ubuntuuitoolkit: Autopilot test cases and CPOs
   ./run_tests.sh list
   ./run_tests.sh run [TESTCASE]
 - api: Tests for apicheck (API description generator)
   Run 'make check' in the folder
 - license: Script to verify that sources contains copright headers
   make license
 - resources: Manual test cases (deprecated, should move to unit/visual)
 - unit:
   Unit tests using QTest/UbuntuTestCase
   Run 'make check[_BASENAME_WITH_UNDERSCORES]' in the respective subfolder
   eg. 'tst_label_extras.13.qml' becomes 'make check_tst_label_extras_13'
   This is driven by runtest.sh and checkresults.sh.
  - visual: All tests in this folder can be run as unit tests as well as
    interactively. That means 'make check' or via gallery.sh.
    Optional: prefix with './tests/xvfb.sh' to run inside XVfb.
    Note for adding new files: The root must be a visual Item-based type
    with a width and height. TestCase or UbuntuTestCase must be a child.

 Verify the whole Toolkit API:
 `./tests/qmlapicheck.sh` inspects components.api and produces components.api.new

 Verify the tidyness of packaging:
 `./packaging-sorting.sh`

 Verify the coding style of Python source code:
 pyflakes .

 `source export_qml_dir.sh` can be used in bash to setup the environment.


## Documentation

Once built, you may view the documentation by opening  
  documentation/html/overview-ubuntu-sdk.html
in your favorite web browser.

## Autopilot SDK Helpers Documentation (Sphinx docs):

To build the Autopilot SDK Helpers documentation, you need to install sphinx.
After installation, change directory to documentation/autopilot-helpers
and executing make.

```
sudo apt-get install python3-sphinx
cd documentation/autopilot-helpers
make singlehtml
```

 * Once built, you may view the documentation by opening
 `_build/singlehtml/index.html` in your favorite web browser.
