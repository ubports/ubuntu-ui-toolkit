#!/bin/bash

./testcase -vs -v1|egrep "QML|Declarative" |egrep -v "updateScene"
