#!/bin/sh
for f in `find $TARGET -type f -name "*.po"`
    do
        FILENAME=${f%.*}
        EXTENSION=${f#*.}
        msgfmt $f -o $FILENAME'.mo'
    done
