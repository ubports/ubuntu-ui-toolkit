#!/bin/bash
export PATH=/opt/qt5/bin:$PATH
cd tests/autopilot

echo running with arg: $1

if [ "$1" == "" ]; then
	autopilot run tavastia
else
	autopilot run -o ../../$1 -f xml -r -rd ../../ tavastia
fi

exit 0

