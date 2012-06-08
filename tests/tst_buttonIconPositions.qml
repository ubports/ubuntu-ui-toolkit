import QtQuick 1.1
import QtQuickTest 1.0
import "../Components"

Button {
    id: button

    iconSource: "../examples/call_icon.png"
    text: "A few words of text"
    width: 250
    height: 80
    iconPosition: "left"

    TestCase {
        name: "Testing Button::iconPosition property"
        when: windowShown

        property Item icon: null
        property Item label: null

        // run before test_* functions executed
        function initTestCase() {
            // locate the 'label' and 'icon' elements inside Button by the fact that
            // we know a property/value pair for each.
            for (var i=0; i < button.children.length; i++) {
                if ("text" in button.children[i] && button.children[i].text == button.text) {
                    label = button.children[i];
                }
                else if ("source" in button.children[i] && button.children[i].source == button.iconSource) {
                    icon = button.children[i];
                }
            }
            if (icon === null || label === null) {
                skipAll("Test error: 'Button' structure changed");
            }
        }

        function test_iconPositionRight() {
            iconPosition = "right";
            compare(label.x + label.width < icon.x, true, "text not to left of icon");
            compare(label.y + Math.ceil(label.height / 2), icon.y + Math.ceil(icon.height / 2), "text & icon not horizontally aligned");
        }

        function test_iconPositionLeft() {
            iconPosition = "left";
            compare(icon.x + icon.width < label.x, true, "text not to right of icon");
            compare(label.y + Math.ceil(label.height / 2), icon.y + Math.ceil(icon.height / 2), "text & icon not horizontally aligned");
        }

        function test_iconPositionTop() {
            iconPosition = "top";
            compare(icon.y + icon.height < label.y, true, "text not underneath icon");
            compare(label.x + Math.ceil(label.width / 2), icon.x + Math.ceil(icon.width / 2), "text & icon not vertically aligned");
        }

        function test_iconPositionBottom() {
            iconPosition = "bottom";
            compare(label.y + label.height < icon.y, true, "text not above icon");
            compare(label.x + Math.ceil(label.width / 2), icon.x + Math.ceil(icon.width / 2), "text & icon not vertically aligned");
        }
    }
}
