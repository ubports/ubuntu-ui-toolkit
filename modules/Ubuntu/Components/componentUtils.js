/*
 * Copyright 2012 Canonical Ltd.
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

/*
  This cannot be made as standalone library as from those we cannot access the
  attached properties.
  */

/*!
  The function checks the existence of a style property.
  */
function hasStyle(object, property) {
    // FIXME: When a module contains QML, C++ and JavaScript elements exported,
    // we need to use named imports otherwise namespace collision is reported
    // by the QML engine. As workaround, we use Theming named import.
    // Bug to watch: https://bugreports.qt-project.org/browse/QTBUG-27645
    return (object && property && object.Theming.ItemStyle.style && (undefined !== object.Theming.ItemStyle.style[property]))
}

/*!
  The function returns the style property specified for the given object. If the
  property is not found, the specified default value is returned.
  Example:
    ComponentUtils.style(button, "color", "transparent")
    ComponentUtils.style(label, "maximumLineCount")
  */
function style(object, property, defaultValue) {
    print("ComponentUtils.style is DEPRECATED, called by " + object);
    // FIXME: When a module contains QML, C++ and JavaScript elements exported,
    // we need to use named imports otherwise namespace collision is reported
    // by the QML engine. As workaround, we use Theming named import.
    // Bug to watch: https://bugreports.qt-project.org/browse/QTBUG-27645
    if (hasStyle(object, property))
        return object.Theming.ItemStyle.style[property];
    return defaultValue;
}

/*!
  The function checks the existence of the delegate property of a given component.
  Returns the property or undefined.
  */
function delegateProperty(object, property, defaultValue) {
    if (object && property && object.Theming.ItemStyle.delegate && (undefined !== object.Theming.ItemStyle.delegate[property]))
        return object.Theming.ItemStyle.delegate[property];
    return defaultValue;
}
