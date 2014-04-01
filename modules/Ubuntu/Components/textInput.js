/*
 * Copyright 2014 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

.pragma library

/*
  Helper functions for text inputs.
  The \a input must have a boolean \a selectionMode property declared. The \a flicker
  must have a \a flickerTimer Timer{} property declared.
  */

function TextInputHelper(input, flicker, im) {
    var moveStarts, moveEnds;
    var pressedPosition = 0;
    var textChanged = false;
    var handlingEnabled = true;
    var flickTimer = (flicker && flicker.hasOwnProperty("flickTimer")) ? flicker.flickTimer: null;
    var longOrDoubleTap = false;
    // only TextInput has validator property
    var singleLine = input.hasOwnProperty("validator");


    // slots
    function toggleFlickerInteractiveMode() {
        if (flicker.autoSizeInfinite) {
            // disable interactive mode
            flicker.interactive =  false;
        } else {
            flicker.interactive = !input.activeFocus;
        }
    }
    function disableFlicking() {
        if (flicker.moving) {
            // do not disable while moving
            print("MOVES STILL")
            return;
        }
        flicker.interactive = false;
        handlingEnabled = true;
    }
    function toggleHandling() {
        handlingEnabled = !flicker.interactive;
    }
    function syncTextChange() {
        textChanged = true;
    }
    function ensureVisible()
    {
        var r = input.cursorRectangle;
        if (flicker.moving || flicker.flicking)
            return;
        if (flicker.contentX >= r.x)
            flicker.contentX = r.x;
        else if (flicker.contentX + flicker.width <= r.x + r.width)
            flicker.contentX = r.x + r.width - flicker.width;
        if (flicker.contentY >= r.y)
            flicker.contentY = r.y;
        else if (flicker.contentY + flicker.height <= r.y + r.height)
            flicker.contentY = r.y + r.height - flicker.height;
    }
    function flickResetSelectionMode() {
        input.selectionMode = false;
        longOrDoubleTap = false;
    }

    // initialization
    input.selectionMode = (input.persistentSelection && input.selectedText !== "")
    input.textChanged.connect(syncTextChange);
    if (flicker) {
        if (!flicker.hasOwnProperty("flickTimer")) {
            console.exception("ERROR: Flickable does not declare flickTimer.");
            return null;
        }
        // do not forward events to contentItem, grab it immediately so we can differentiate the flick from selection
        flicker.pressDelay = 0;
        input.activeFocusChanged.connect(toggleFlickerInteractiveMode);
        flicker.interactiveChanged.connect(toggleHandling);
        input.cursorRectangleChanged.connect(ensureVisible);
        flicker.flickStarted.connect(flickResetSelectionMode);
        flicker.movementEnded.connect(disableFlicking);
        // flick timer to distinguish between text selection and flick scrolling
        // if mouse move happens in the first few ms after the press, the event is considered a flicking
        if (flickTimer) {
            flickTimer.interval = 200;
            flickTimer.repeat = false;
            flickTimer.onTriggered.connect(disableFlicking);
            flickTimer.running = false;
        }
    }

    /*-----------------------PRIVATE----------------------- */
    /*
      Resets the selection boundaries.
      */
    function resetSelectionBoundaries(mouse) {
        input.cursorPosition =
                singleLine ? input.positionAt(mouse.x) : input.positionAt(mouse.x, mouse.y);
    }

    /*
      Reset selection boundaries and cursor position.
      */
    function resetCursors(mouse) {
        resetSelectionBoundaries(mouse);
        input.cursorPosition = pressedPosition = singleLine ?
                    input.positionAt(mouse.x) : input.positionAt(mouse.x, mouse.y);
        moveStarts = moveEnds = undefined;
    }

    /*
      Moves the selection.
      */
    function moveSelection(mouse) {
        moveEnds = singleLine ? input.positionAt(mouse.x) : input.positionAt(mouse.x, mouse.y);
        if (moveStarts === undefined) {
            moveStarts = input.selectionStart;
        }

        if (input.selectionMode) {
            input.select(moveStarts, moveEnds);
        }
    }

    /*-----------------------PUBLIC----------------------- */
    /*
      Activate input panel, and open the OSK if was hidden.
      */
    this.activateInput = function () {
        if (!input.activeFocus) {
            input.forceActiveFocus();
        } else {
            this.showInputPanel();
        }
    }

    /*
      Shows the OSK.
      */
    this.showInputPanel = function () {
        if (!im.visible) {
            im.show();
        }
        textChanged = false;
    }

    /*
      Hides the OSK.
      */
    this.hideInputPanel = function () {
        im.hide();
        // emit accepted signal if changed
        if (textChanged && input.hasOwnProperty("accepted")) {
            input.accepted();
        }
    }

    /*
      Checks whether the mouse fall in the selected text area.
      */
    this.mouseInSelection = function (mouse) {
        var pos = singleLine ?
                    input.positionAt(mouse.x) : input.positionAt(mouse.x, mouse.y);
        return (input.selectionStart !== input.selectionEnd)
                && (pos >= Math.min(input.selectionStart, input.selectionEnd))
                && (pos <= Math.max(input.selectionStart, input.selectionEnd));
    }

    /*
      Resets selection mode due to key interaction.
      */
    this.resetSelectionMode = function() {
        flickResetSelectionMode();
    }

    /*-----------------------INPUT ACTIONS----------------------- */
    /*
      on pressed, remembers the press position
      */
    this.pressed = function (mouse) {
        pressedPosition = singleLine ?
                    input.positionAt(mouse.x) : input.positionAt(mouse.x, mouse.y);
        // leave selection mode if we are outside the selection area
        input.selectionMode = this.mouseInSelection(mouse);
        if (input.activeFocus && flicker && flickTimer) {
            // enable flicker and start flick-timer
            flicker.interactive = true;
            flickTimer.restart();
        }
    }

    /*
      Activates input, resets selection if needed
      */
    this.released = function (mouse) {
        this.activateInput();
        if (flickTimer) {
            // stop flick timer
            flickTimer.running = false;
        }

        if (longOrDoubleTap && this.mouseInSelection(mouse)) {
            return;
        }
        longOrDoubleTap = false;
        resetCursors(mouse);
        input.selectionMode = false;
    }

    /*
      Performs selection
      */
    this.positionChanged = function (mouse, hoverEnabled) {
        if (!input.activeFocus || mouse.button !== Qt.LeftButton) {
            return;
        }
        if (flicker && flicker.interactive) {
            if (flickTimer) {
                // make sure flick timer is not running anymore
                flickTimer.running = false;
            }
            return;
        }

        if (!input.selectionMode) {
            input.cursorPosition = pressedPosition;
            resetSelectionBoundaries(mouse);
            input.selectionMode = true;
        }

        moveSelection(mouse);
    }

    /*
      Selects a word under cursor. The input is activated and its cursor is positioned by a previous
      click.
      */
    this.doubleTap = function (mouse) {
        longOrDoubleTap = true;
        input.selectWord();
        pressedPosition = input.selectionEnd;
    }

    /*
      Performs pressAndHold action. Returns true if the action can be performed
      */
    this.pressAndHold = function (mouse) {
        if (!input.activeFocus || !handlingEnabled) {
            return false;
        }
        if (!this.mouseInSelection(mouse)) {
            return false;
        }
        return true;
    }
}
