
function style(object, proeprty, defaultValue) {
    if (undefined !== object.Theming.ItemStyle.style[proeprty])
        return object.Theming.ItemStyle.style[proeprty];
    return defaultValue;
}
