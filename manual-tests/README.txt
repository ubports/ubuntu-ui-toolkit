==============
Manual Testing
==============

Outline
-------
Sometimes it is not possible to easily make an automated test for some
use-cases.  However just because something isn't easily tested automatically
doesn't mean that there isn't value in creating a test for it, just that the
test needs to be executed by a person until it is converted into an automated
test.

However please be very discriminatory about your manual tests. 
It's highly likely that you can automate whatever it is you're testing, 
and doing so will be of much more value to the quality of UI Toolkit.
Manual tests are a last resort. They should not be written lightly,
and should give you a feeling of shame.

If you're to the point where you believe you need a manual test,
then please come talk to the QA team and we will see if we can find an automatable solution.


Format
------
Manual tests take the format of text files in this directory (the manual-test
one).  These files are formatted using `reStructured Text`_.  A very
comprehensive `quick reference`_ is available.

.. _reStructured Text: http://en.wikipedia.org/wiki/ReStructuredText
.. _quick reference: http://docutils.sourceforge.net/docs/user/rst/quickref.html

Tests have a header, actions (steps to follow), and an expected result.
You can use the TEST_TEMPLATE.txt file as template for new tests.

There can be multiple tests in a single file, but they should all be related,
and the filename should indicate the tests it contains.

Example:

Test Button
---------
This test shows that the button emits a signal when pressed

Setup:
#. start examples/ubuntu-ui-toolkit-gallery/ubuntu-ui-toolkit-gallery.qml in qmlscene 

Actions:
#. Select Buttons
#. Press and hold LMB on top of the QML Button next to the Label which says "Standard"
#. Release LMB

Expected Result:
#. Buttons page is visible
#. QML Button is in pressedColor while LMB is pressed.
#. QML Button emits signal clicked() when LMB is released.


Directory Structure
-------------------
It is expected that as we grow a number of manual tests, we will use
directories to organise them.


=========
Glossary
=========
LMB = Left Mouse Button


=========
HOW TO
=========


How To See Which Testcase Tests Which Part Of The QML Component? =
----------
open .map file in text editor


How To Get The Testcase Executor Binary
----------
/compile_testcaseexecutor.sh


How To Execute Tests
----------
./run_testcases.sh
