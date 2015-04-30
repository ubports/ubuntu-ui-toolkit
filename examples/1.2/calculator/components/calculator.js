/*
 * Copyright 2013 Canonical Ltd.
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
 *
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

.pragma library

function Calculator(textInput, memoryIndicator) {
    var operationStage = 0; // 0-lvalue, 1-rvalue
    var lvalue = 0.0;
    var rvalue = 0.0;
    var lastOperation = -1;
    var memory = 0.0;
    var clearInputNext = true;

    textInput.text = lvalue;
    memoryIndicator.visible = false;

    function setValue(value) {
        if (operationStage === 0) {
            lvalue = parseFloat(value);
            return lvalue;
        } else if (operationStage === 1) {
            rvalue = parseFloat(value);
            return rvalue;
        }
    }
    function value() {
        return (operationStage === 0) ? lvalue : rvalue;
    }

    function clear() {
        operationStage = 1;
        lvalue = rvalue = 0;
        lastOperation = -1;
        textInput.text = 0.0;
    }
    function memClear() {
        memory = 0;
        memoryIndicator.visible = false;
    }
    function memRecall() {
        if (memory <= 0)
            return;
        textInput.text = setValue(memory);
    }
    function memAdd() {
        memory += value();
        textInput.text = setValue(memory);
        memoryIndicator.visible = true;
    }
    function memDec() {
        memory -= value();
        textInput.text = setValue(memory);
        memoryIndicator.visible = true;
    }

    // arythmetic functions
    function add() {
        lvalue += rvalue;
    }
    function dec() {
        lvalue -= rvalue;
    }
    function mul() {
        lvalue *= rvalue;
    }
    function div() {
        lvalue /= rvalue;
    }

    function sign() {
        textInput.text = setValue(-value());
    }

    function invert() {
        textInput.text = setValue(1/value());
    }
    function percent() {
        textInput.text = setValue(value() / 100);
    }
    function sqr() {
        textInput.text = setValue(Math.pow(value(), 2));
    }
    function cube() {
        textInput.text = setValue(Math.pow(value(), 3));
    }
    function power() {
        lvalue = Math.pow(lvalue, rvalue);
    }
    function powerOf2() {
        textInput.text = setValue(Math.pow(2, value()));
    }
    function factorial() {
        var rval = 1;
        for (var i = 2; i <= value(); i++) {
            rval *= i;
        }
        textInput.text = setValue(rval);
    }
    function sqrt() {
        textInput.text = setValue(Math.sqrt(value()));
    }
    function ln() {
        textInput.text = setValue(Math.log(value()));
    }
    function exp()
    {
        textInput.text = setValue(Math.exp(value()));
    }
    function sin() {
        textInput.text = setValue(Math.sin(value()));
    }
    function cos() {
        textInput.text = setValue(Math.cos(value()));
    }
    function tan() {
        textInput.text = setValue(Math.tan(value()));
    }
    function cot() {
        textInput.text = setValue(1/Math.tan(value()));
    }
    function sec() {
        textInput.text = setValue(1/Math.cos(value()));
    }
    function csc() {
        textInput.text = setValue(1/Math.sin(value()));
    }

    var operations = [
                {code: "+", operationFunc: add},
                {code: "-", operationFunc: dec},
                {code: "*", operationFunc: mul},
                {code: "/", operationFunc: div},

                {code: "+/-", controlFunc: sign},
                {code: "c", controlFunc: clear},

                {code: "mc", controlFunc: memClear},
                {code: "mr", controlFunc: memRecall},
                {code: "m+", controlFunc: memAdd},
                {code: "m-", controlFunc: memDec},

                {code: "%", controlFunc: percent},
                {code: "1/x", controlFunc: invert},
                {code: "x!", controlFunc: factorial},
                {code: "x^2", controlFunc: sqr},
                {code: "x^3", controlFunc: cube},
                {code: "x^y", operationFunc: power},
                {code: "2^x", controlFunc: powerOf2},
                {code: "sqrt", controlFunc: sqrt},
                {code: "ln", controlFunc: ln},
                {code: "e^x", controlFunc: exp},
                {code: "sin()", controlFunc: sin},
                {code: "cos()", controlFunc: cos},
                {code: "tan()", controlFunc: tan},
                {code: "cot()", controlFunc: cot},
                {code: "sec()", controlFunc: sec},
                {code: "csc()", controlFunc: csc},

            ];

    this.execute = function (operation) {
        if (operation === "=") {
            if (lastOperation > -1) {
                operations[lastOperation].operationFunc();
                textInput.text = lvalue;
            }
            operationStage = 0;
            clearInputNext = true;
        } else {
            for (var i = 0; i < operations.length; i++) {
                if (operations[i].code === operation) {
                    if (operations[i].operationFunc) {
                        clearInputNext = true;
                        if (operations[i].singleOp) {
                            operations[i].operationFunc();
                            return;
                        }

                        lastOperation = i;
                        if (operationStage === 0) {
                            operationStage = 1;
                        } else if (operationStage === 1) {
                            // evaluate and continue receiving rvalue
                            operations[i].operationFunc(lvalue, rvalue);
                            textInput.text = lvalue;
                        }
                    } else if (operations[i].controlFunc) {
                        operations[i].controlFunc();
                        clearInputNext = true;
                    }

                    return;
                }
            }

            // if no opeartion was performed, add operation to input
            if (clearInputNext) {
                clearInputNext = false;
                textInput.text = "";
            }
            textInput.text = setValue(textInput.text + operation);
        }
    }
}
