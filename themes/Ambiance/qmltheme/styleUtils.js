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

/* Checks whether the property is defined in the itemStyle, and if found, returns
  the property value or the given (optional) default value.
  Example:
     itemStyleProperty("color", Qt.rgba(0,0,0,0))
     itemStyleProperty("background") - returns "undefined" if the background
        property is not found.
 */
function itemStyleProperty(property, defaultValue) {
    if (undefined !== itemStyle[property]) {
        return itemStyle[property];
    }
    return defaultValue;
}

/* Checks whether the animation property is defined in the itemStyle, and sets
   the animation targets to the given target. The function can be used when the
   delegates use the animations defined in the style straight (meaning not embedded
   in Transitions or Behaviors).

   Parameters:
    - animation: animation property in the itemStyle
    - target: target object to apply the value/animation on
    - proeprty: property in the target animated or set
    - endValue: property value to be set if the animation is not specified in the style
  */
function animateOrSetValue(animation, target, property, endValue)
{
    if (!animate(animation, target))
        target[property] = endValue;
}


/* Run an animation from the style. This can be used from Behaviors as follows:
   Behavior on property {
       ScriptAction {
           script: StyleUtils.animate("animationProperty")
       }
   }
  */
function animate(property, target) {
    if (itemStyle[property]) {
        var anim = itemStyle[property];
        // stop the previously running animation
        if (anim.running) {
            anim.stop();
        }
        if (undefined !== target) {
            // set the target for the animation
            if (undefined !== anim['target'] && anim.target !== target) {
                anim.target = target;
            }
            // and also for the child animations
            for (var i = 0; (undefined !== anim.animations) && (i < anim.animations.length); i++) {
                if (undefined !== anim.animations[i]['target']) {
                    anim.animations[i].target = target;
                }
            }
        }

        // never run the animation till the end
        anim.alwaysRunToEnd = false;
        anim.restart();
        return true;
    }
    return false;
}
