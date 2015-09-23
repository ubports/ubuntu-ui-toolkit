#!/bin/bash
# Copyright 2014 Canonical Ltd.
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
# Author: Zoltán Balogh <zoltan.baloghn@canonical.com>


SERIALNUMBER=086e443edf51b915
PPA="ubuntu-sdk-team/staging"
TIMESTAMP=`date +"%Y_%m_%d-%H_%M_%S"`
DATESTAMP=`date +"%Y_%m_%d"`
LOGFILENAME="ap-${TIMESTAMP}"
OUTPUTDIR=$HOME
RTM=true
REVISION=105
DISTRO="ubuntu"
SERIES="vivid"
CHANNEL="ubuntu-touch/rc-proposed/${DISTRO}"
PASSWORD="0000"
BOOTTIME=250
COMISSION=false
APPLICATION="dialer-app"
COUNT=10
SLEEP_TIME=5


WIRELESS_ADAPTER="$(nmcli -t -f device,type dev | egrep "wireless|wifi" | cut -d: -f1)"

IP_ADDRESS="$(ifconfig | grep -A 1 ${WIRELESS_ADAPTER} | tail -1 | cut -d ':' -f 2 | cut -d ' ' -f 1)"


declare -a APPLICATIONS=(
        "dialer-app"
)

sleep_indicator () {
    if [ -z "$1" ]; then
        i=1
        while [  $(adb get-state  -s ${SERIALNUMBER}) == "unknown" ]
        do
                echo -ne "Wait for adb: $i seconds\r"
                sleep 1
                i=$[$i+1]
        done
        echo -ne '\n'
        until pids=$(adb -s ${SERIALNUMBER} shell pidof unity-system-compositor|egrep -v "^$|initctl:"); 
        do
          echo -ne "Wait for compositor: $i seconds\r"
          sleep 1;
          i=$[$i+1]
        done;
        echo -ne '\n'
        return
    fi
    SLEEP=$1
    for (( LOOPVAR=1; LOOPVAR<= ${SLEEP}; LOOPVAR++ ))
    do
        echo -ne "Wait: $SLEEP/$LOOPVAR seconds\r"
        sleep 1
    done
    echo -ne '\n'
    return 0
}

function unlock_screen {
        adb -s ${SERIALNUMBER} shell powerd-cli display on |egrep -v "Display State requested, cookie is|Press ctrl-c to exit|not fully supported." &
        adb -s ${SERIALNUMBER} shell powerd-cli active |egrep -v "requested, cookie is|Press ctrl-c to exit|not fully supported." &
        sleep_indicator 10
        adb -s ${SERIALNUMBER} shell amixer -D pulse set Master 1+ mute 2>&1 > /dev/null
        adb -s ${SERIALNUMBER} shell "gdbus call --session --dest com.canonical.UnityGreeter --object-path / --method com.canonical.UnityGreeter.HideGreeter|grep -v '\(\)'"
        adb -s ${SERIALNUMBER} shell "echo ${PASSWORD}|sudo -S dbus-send   --system --print-reply \
                                                 --dest=org.freedesktop.Accounts \
                                                 /org/freedesktop/Accounts/User32011 \
                                                 org.freedesktop.DBus.Properties.Set \
                                                 string:com.canonical.unity.AccountsService \
                                                 string:demo-edges variant:boolean:false 2>&1|grep -v password|egrep -v '\(\)|method return'"
}


function network {
        if phablet-network -s ${SERIALNUMBER} 2>&1|grep -q Error; then
                echo "Reset and try again."
                reset -f
                phablet-network -s ${SERIALNUMBER} 2>&1
        fi
}


function device_provisioning {
	# Use the first available device for testing
        echo -e "Waiting for a device"
        adb wait-for-device
        SERIALNUMBER=`adb devices -l | grep -Ev "List of devices attached" | grep -Ev "emulator-" | sed "/^$/d"|sed "s/ .*//g"`
	adb -s ${SERIALNUMBER} wait-for-device
        # Avoid https://bugs.launchpad.net/gallery-app/+bug/1363190
        adb -s ${SERIALNUMBER} shell "echo ${PASSWORD} |sudo -S rm -rf /userdata/user-data/phablet/.cache/com.ubuntu.gallery 2>&1|grep -v password"
        # flash the latest image
        echo -e "Flashing \e[31m${CHANNEL}\e[0m"
        ubuntu-device-flash touch --serial=${SERIALNUMBER} --channel=${CHANNEL} --wipe --developer-mode --password=${PASSWORD}
	sleep_indicator ${BOOTTIME}
	echo -e "Disable the intro wizard"
	phablet-config -s ${SERIALNUMBER}  welcome-wizard --disable
	sleep_indicator 10
	echo -e "Disable the edge swiping lecture "
	phablet-config -s ${SERIALNUMBER} edges-intro --disable	
        sleep_indicator 10
        echo -e "Clone the network "
        network
	sleep_indicator 10
        echo "Set up with the archive image"
        phablet-config -s ${SERIALNUMBER} writable-image -r ${PASSWORD} 2>&1 > /dev/null
        echo "Sleep after phablet-config";
        sleep_indicator 120
	echo -e "Clone the network "
	network
	adb -s ${SERIALNUMBER} shell "echo ${PASSWORD}|sudo -S reboot 2>&1|grep -v password"
	sleep_indicator 120
}

while getopts ":hco:" opt; do
    case $opt in
        c)
            COMISSION=true
            ;;
        o)
            if [ -d "$OPTARG" ]; then
                OUTPUTDIR=$OPTARG
            else
                echo "$OPTARG does not exist. Using default $OUTPUTDIR"
            fi
            ;;

        h)
            echo "Usage: appstart_profiling_test_plan.sh -c -o [ PATH ]"
            echo -e "\t-c : Comission the device with the ${PPA} enabled"
            echo -e "\t-o : Output directory. Default $OUTPUTDIR"
            exit
	    ;;
        :)
            echo "Option -$OPTARG requires an argument." >&2
            exit
    esac
done




# Flash the device with rc-proposed
if [ ${COMISSION} == true ]; then
	device_provisioning
fi

# Unlock the screan
echo -e "Unlocking the screan"
#unlock_screen

# Start the lttng-relayd if needed
pgrep lttng-relayd > /dev/null
if [ $? -eq 0 ]; then
	echo -e "The lttng-relayd is already running."
else
	echo -e "Starting lttng-relayd"
	lttng-relayd &
	sleep_indicator 5

fi

nc -z ${IP_ADDRESS} 5343
if [ $? -eq 0 ]; then
	echo "The lttng server ${URL} is listening on 5343"
else
        echo "The lttng server is not accesible. Check lttng-relayd or firewall policies."
fi

for APPLICATION in "${APPLICATIONS[@]}"
    do
        echo -e "\e[31m${APPLICATION}\e[0m"
	adb -s ${SERIALNUMBER} shell "/usr/bin/profile_appstart.sh -a ${APPLICATION} -u ${IP_ADDRESS} -c ${COUNT} -s ${SLEEP_TIME}"
    done

# Measure the application startup times

# Configure the PPA on the devoce

# Measure the application startup times

echo ${IP_ADDRESS}

