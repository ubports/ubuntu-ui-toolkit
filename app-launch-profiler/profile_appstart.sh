#!/bin/bash
# Copyright 2015 Canonical Ltd.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; version 2.1.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Author: Benjamin Zeller <benjamin.zeller@canonical.com>
#         Zoltán Balogh <zoltan.balogh@canonical.com 

URL=127.0.0.1
APP_NAME=dialer-app
COUNT=100
SLEEP_TIME=10

while getopts ":u:a:c:s:h" opt; do
    case $opt in
        h)
            echo "Usage: profile_appstart.sh -u [IP address|local if left empty] -c [count] -a [app] -s [sleep timme]"
            echo -e "\t-u : The IP address of the profiler:. Default: ${URL}"
            echo -e "\t-a : The name of the application. Default: ${APP_NAME}"
            echo -e "\t-c : Number of times the app is started. Default: ${COUNT}"
            echo -e "\t-s : Length of slep between app starts. Default: ${SLEEP_TIME}"
            exit
            ;;
        u)
            URL=$OPTARG
            ;;
        a)
            APP_NAME=$OPTARG
            ;;
        c)
            COUNT=$OPTARG
            ;;
        s)
            SLEEP_TIME=$OPTARG
            ;;
        :)
            echo "Option -$OPTARG requires an argument." >&2
            exit
    esac
done

echo -e "URL=\t${URL}"
echo -e "APP_NAME=\t${APP_NAME}"
echo -e "COUNT=\t${COUNT}"
echo -e "SLEEP_TIME\t${SLEEP_TIME}"
if [ ${URL} == local ]; then
	lttng create
else
	lttng create --set-url net://${URL}
fi

lttng enable-event --userspace qtmir:firstFrameDrawn
lttng enable-event --userspace app:*
lttng start

for x in $(seq 1 ${COUNT}); do
    echo 1 > /proc/sys/vm/drop_caches
    sleep 1
    su - phablet -c "/usr/bin/app-launch-tracepoints"
    su - phablet -c "ubuntu-app-launch ${APP_NAME}"
    sleep ${SLEEP_TIME}
    su - phablet -c "ubuntu-app-stop ${APP_NAME}"
done

lttng stop
