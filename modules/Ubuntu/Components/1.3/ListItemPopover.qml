import QtQuick 2.4
import Ubuntu.Components.Popups 1.3

ActionSelectionPopover {
    function mergeActions(to, from) {
        if (from == null)
            return;

        var actions = from.actions;
        for (var i in actions) {
            var action = actions[i];
            if (!action.text)
                action.text = action.iconName;
            to.push(actions[i]);
        }
    }

    actions: {
        var all = [];
        mergeActions(all, caller.leadingActions);
        mergeActions(all, caller.trailingActions);
        return all;
    }

    // Hide the arrow
    Binding {
        target: __foreground
        property: "direction"
        value: "none"
    }
}
