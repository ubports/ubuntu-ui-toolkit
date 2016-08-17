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

URL=192.168.1.125
APP_NAME=dialer-app
COUNT=100
SLEEP_TIME=10
PASSWORD="0000"
WITH_CACHE=false

while getopts ":u:p:a:c:s:qh" opt; do
    case $opt in
        h)
            echo "Usage: profile_appstart.sh -u [IP address|local if left empty] -p [phabelt password] -c [count] -a [app] -s [sleep timme] -q"
            echo -e "\t-u : The IP address of the profiler:. Default: ${URL}"
            echo -e "\t-p : Password of the phablet user. Default: ${PASSWORD}"
            echo -e "\t-a : The name of the application. Default: ${APP_NAME}"
            echo -e "\t-c : Number of times the app is started. Default: ${COUNT}"
            echo -e "\t-s : Length of slep between app starts. Default: ${SLEEP_TIME}"
	    echo -e "\t-q : Run tests with QML cache enabled. Default: ${WITH_CACHE}"
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
	q)
	    WITH_CACHE=true
            ;;
        :)
            echo "Option -$OPTARG requires an argument." >&2
            exit
    esac
done

if [ ${URL} == local ]; then
	echo ${PASSWORD}|sudo -S lttng create
else
    	nc -z ${URL} 5343
    	if [ $? -eq 0 ]; then
        	echo "The lttng server ${URL} is listening on 5343"
		echo ${PASSWORD}|sudo -S bash -c "lttng create --set-url net://${URL}"	
    	else
       		echo "The lttng server is not accesible. Check lttng-relayd or firewall policies."
		echo "Falling back to local"
		URL="local"
	        echo ${PASSWORD}|sudo -S lttng create
	
    	fi
fi

echo ${PASSWORD}|sudo -S bash -c 'lttng enable-event --userspace qtmir:firstFrameDrawn'
echo ${PASSWORD}|sudo -S bash -c 'lttng enable-event --userspace app:*'
echo ${PASSWORD}|sudo -S bash -c 'lttng start'

for x in $(seq 1 ${COUNT}); do
    echo ${PASSWORD}|sudo -S bash -c 'echo 1 > /proc/sys/vm/drop_caches'
    sleep 5
    echo "x - $x"
    if [[  $WITH_CACHE == false ]]; then
      # Cleaning QML cache  
      rm -rf ~/.cache/QML/Apps/${APP_NAME}
    fi
    echo ${PASSWORD}|sudo -S /usr/bin/app-launch-tracepoints
    ubuntu-app-launch ${APP_NAME}
    sleep ${SLEEP_TIME}
    ubuntu-app-stop ${APP_NAME}
done

echo ${PASSWORD}|sudo -S bash -c 'lttng stop'
