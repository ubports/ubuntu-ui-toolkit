.pragma library

// the function converts the value entered in the input to the other units
function convert(input, converter, index) {
    if (input && input.text.length <= 0)
        return;
    var baseValue = parseFloat(input.text) / parseFloat(converter.model[index].rate);
    if (baseValue <= 0.0)
        return;
    for (var i = 0; i < converter.count; i++) {
        if (converter.itemAt(i)) {
            var inputItem = converter.itemAt(i).children[1]
            if (inputItem !== input) {
                inputItem.text = parseFloat(converter.model[i].rate) * baseValue;
            }
        }
    }
}

// the function clears every input field data
function clear(converter, except) {
    for (var i = 0; i < converter.count; i++) {
        if (!except || (except && except !== converter.itemAt(i).children[1]))
            converter.itemAt(i).children[1].text = "0.0";
    }
}
