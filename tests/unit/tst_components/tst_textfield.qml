import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 0.1

TestCase {
     name: "TextFieldAPI"

     function test_acceptableInput() {
         compare(textField.acceptableInput,true,"acceptableInput true by default")
     }

     function test_cursorPosition() {
         compare(textField.cursorPosition,0,"cursorPosition 0 by default")
     }

     function test_customSoftwareInputPanel() {
         compare(textField.customSoftwareInputPanel,null,"customSoftwareInputPanel is null by default")
     }

     function test_echoMode() {
         compare(textField.echoMode, TextInput.Normal,"echoMode is TextInput.Normal by default")
     }

     function test_errorHighlight() {
         compare(textField.errorHighlight, false,"errorHighlight is false by default")
         textField.errorHighlight = true
         compare(textField.errorHighlight,true,"set/get")
     }

     function test_font() {
         verify((textField.font),"font is set")
     }

     function test_hasClearButton() {
         compare(textField.hasClearButton, true, "hasClearButton is false by default")
         textField.hasClearButton = false
         compare(textField.hasClearButton, false, "set/get")
     }

     function test_inputMask() {
         compare(textField.inputMask, "", "inputMask is undefined by default")
     }

     function test_inputMethodComposing() {
         compare(textField.inputMethodComposing, false, "inputMethodComposing is false by default")
     }

     function test_maximumLength() {
         compare(textField.maximumLength, 32767, "maximumLength is 32767 by default")
     }

     function test_placeholderText() {
         compare(textField.placeholderText, "", "placeholderText is '' by default")
     }

     function test_primaryItem() {
         compare(textField.primaryItem, undefined, "primaryItem is undefined by default")
     }

     function test_readOnly() {
         compare(textField.readOnly, false, "readOnly is false by default")
         textField.readOnly = true
         compare(textField.readOnly, true, "set/get")
     }

     function test_secondaryItem() {
         compare(textField.secondaryItem, undefined, "secondaryItem is undefined by default")
     }

     function test_selectedText() {
         compare(textField.selectedText, "", "selectedText is '' by default")
     }

     function test_selectionEnd() {
         compare(textField.selectionEnd, 0, "selectionEnd is 0 by default")
     }

     function test_selectionStart() {
         compare(textField.selectionStart, 0, "selectionStart is 0 by default")
     }

     function test_text() {
         compare(textField.text, "", "text is '' by default")
         var newText = "Hello World!"
         textField.text = newText
         compare(textField.text, newText, "set/get")
     }

     function test_validator() {
         compare(textField.validator, null, "validator is null by default")
         textField.validator = regExpValidator
         compare(textField.validator, regExpValidator, "set/get")
     }

     function test_validator_and_acceptableInput_with_invalid_value() {
         textField.validator = null
         compare(textField.acceptableInput,true,"acceptableInput should be true")
         textField.validator = regExpValidator
         textField.text = "012345"
         compare(textField.acceptableInput,false,"with validator failure the acceptableInput should be false")
     }

     RegExpValidator {
         id: regExpValidator
         regExp: /[a-z]*/
     }

     TextField {
         id: textField
     }
}
