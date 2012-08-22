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

However please be very discriminatory about your manual tests. It's highly likely that you can automate whatever it is you're testing, and doing so will be of much more value to the quality of Unity. Manual tests are a last resort. They should not be written lightly, and should give you a feeling of shame. If you're to the point where you believe you need a manual test, then please come talk to the QA team and we will see if we can find an automatable solution.


Format
------
Manual tests take the format of text files in this directory (the manual-test
one).  These files are formatted using `reStructured Text`_.  A very
comprehensive `quick reference`_ is available.

.. _reStructured Text: http://en.wikipedia.org/wiki/ReStructuredText
.. _quick reference: http://docutils.sourceforge.net/docs/user/rst/quickref.html

Tests have a header, actions (steps to follow), and an expected result. You can use the TEST_TEMPLATE.txt file as template for new tests.

There can be multiple tests in a single file, but they should all be related,
and the filename should indicate the tests it contains.

Example:

Test Dash
---------
This test shows that the dash appears when the super key is pushed.

Setup:
#. start an ubuntu (unity) session by logging in

Actions:
#. Start with a clear screen
#. Press the <super> key

Expected Result:
  The dash appears, and focus is in the search box.  The icons on the laucher
  are desaturated except for the ubuntu button at the top.  The icons in the
  panel go white.


Directory Structure
-------------------
It is expected that as we grow a number of manual tests, we will use
directories to organise them.


=========
Glossary
=========
Tap - to press the indicated key and release it, within a timeframe of 50ms.
Pip - one pip is defined as a single filled in arrow to the left of a launcher icon.
