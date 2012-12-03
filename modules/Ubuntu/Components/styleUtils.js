
function style(object, property, defaultValue) {
    if (undefined !== object.Theming.ItemStyle.style[property])
        return object.Theming.ItemStyle.style[property];
    return defaultValue;
}
