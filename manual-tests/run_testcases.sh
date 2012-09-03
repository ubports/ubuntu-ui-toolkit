#!/bin/bash

./testcaseexecutor/testcase -vs -v1|egrep "QML|Declarative" |egrep -v "updateScene"
