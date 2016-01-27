#!/bin/bash
# Copyright 2014 Canonical Ltd.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; version 2.1.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.If not, see <http://www.gnu.org/licenses/>.
#
# Author: Zoltán Balogh <zoltan.baloghn@canonical.com>


SERIALNUMBER=086e443edf51b915
PPA="ubuntu-sdk-team/staging"
TEST_PPA=false
TIMESTAMP=`date +"%Y_%m_%d-%H_%M_%S"`
DATESTAMP=`date +"%Y_%m_%d"`
LOGFILENAME="ap-${TIMESTAMP}"
OUTPUTDIR=$PWD
RTM=true
REVISION=105
DISTRO="ubuntu"
SERIES="vivid"
CHANNEL="ubuntu-touch/rc-proposed/${DISTRO}"
PASSWORD="0000"
BOOTTIME=250
COMISSION=false
COUNT=3
SLEEP_TIME=10
LTTNG_SESSION_NAME_REGEXP='(auto-.*-.*) created.'
LTTNG_SESSION_NAME=""
PPA="ubuntu-sdk-team/testing"

WIRELESS_ADAPTER="$(nmcli -t -f device,type dev | egrep "wireless|wifi" | cut -d: -f1)"
IP_ADDRESS="$(ifconfig | grep -A 1 ${WIRELESS_ADAPTER} | tail -1 | cut -d ':' -f 2 | cut -d ' ' -f 1)"


declare -a APPLICATIONS=(
	"dialer-app"
	"messaging-app"
	"address-book-app"
	"ubuntu-system-settings"
)

sleep_indicator () {
	if [ -z "$1" ]; then
		i=1
		while [$(adb get-state-s ${SERIALNUMBER}) == "unknown" ]
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
	adb -s ${SERIALNUMBER} shell "echo ${PASSWORD}|sudo -S dbus-send --system --print-reply \
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
	# flash the latest image
	echo -e "Flashing \e[31m${CHANNEL}\e[0m"
	ubuntu-device-flash touch --serial=${SERIALNUMBER} --channel=${CHANNEL} --wipe --developer-mode --password=${PASSWORD}
	sleep_indicator ${BOOTTIME}
	echo -e "Disable the intro wizard"
	phablet-config -s ${SERIALNUMBER} welcome-wizard --disable
	sleep_indicator 10
	echo -e "Disable the edge swiping lecture "
	phablet-config -s ${SERIALNUMBER} edges-intro --disable	
	sleep_indicator 10
	echo -e "Clone the network "
	network
        sleep_indicator 10
        adb -s ${SERIALNUMBER} shell "echo ${PASSWORD}|sudo -S reboot 2>&1|grep -v password"
        sleep_indicator 120
	network
        sleep_indicator 10
	echo "Set up with the archive image"
        adb -s ${SERIALNUMBER} shell "echo ${PASSWORD}|sudo -S touch /userdata/.writable_image 2>&1|grep -v password > /dev/null"
        adb -s ${SERIALNUMBER} shell "echo ${PASSWORD}|sudo -S touch /userdata/.adb_onlock 2>&1|grep -v password > /dev/null"
	#phablet-config -s ${SERIALNUMBER} writable-image -r ${PASSWORD} 2>&1 > /dev/null
	echo "Sleep after phablet-config";
	sleep_indicator 10
	echo -e "Clone the network "
	network
	adb -s ${SERIALNUMBER} shell "echo ${PASSWORD}|sudo -S reboot 2>&1|grep -v password"
	sleep_indicator 120
	network
        sleep_indicator 10
}

function measure_app_startups {
	for APPLICATION in "${APPLICATIONS[@]}"
	do
 		echo -e "\e[31m${APPLICATION}\e[0m"
		LTTNG_SESSION_NAME=""
		while read -r LINE
		do
			if [[ $LINE =~ $LTTNG_SESSION_NAME_REGEXP ]]; then
				LTTNG_SESSION_NAME=${BASH_REMATCH[1]}
			fi
		done < <(adb -s ${SERIALNUMBER} shell "/usr/bin/profile_appstart.sh -a ${APPLICATION} -u ${IP_ADDRESS} -c ${COUNT} -s ${SLEEP_TIME}")
		[ -z "${LTTNG_SESSION_NAME}" ] && echo "The lttng session is not available" || app-launch-profiler-lttng.py ~/lttng-traces/ubuntu-phablet/${LTTNG_SESSION_NAME}
	done
} 

while getopts ":hptc:o:" opt; do
	case $opt in
	p)
		COMISSION=true
		;;
	o)
		if [ -d "$OPTARG" ]; then
			OUTPUTDIR=$OPTARG
		else
			echo "$OPTARG does not exist. Using default $OUTPUTDIR"
		fi
		;;
	c)
		COUNT=$OPTARG
		;;
	s)
		SLEEP_TIME=$OPTARG
		;;
	t)
		TEST_PPA=true
		;;
	h)
		echo "Usage: appstart_profiling_test_plan.sh -p -o [ PATH ]"
		echo -e "\t-p : Provision the device with the ${PPA} enabled"
		echo -e "\t-o : Output directory. Default $OUTPUTDIR"
		echo -e "\t-c : Number of times the applications are started during the test. Default $COUNT"
		echo -e "\t-s : Sleep time between application starts. Default $SLEEP_TIME"
		echo -e "\t-t : Run tests against a PPA. Default is ${TEST_PPA} the default PPA is: ${PPA}"
		exit
		;;
	:)
		echo "Option -$OPTARG requires an argument." >&2
		exit
	esac
done


# Use the first available device for testing
echo -e "Waiting for a device"
adb wait-for-device
SERIALNUMBER=`adb devices -l | grep -Ev "List of devices attached" | grep -Ev "emulator-" | sed "/^$/d"|sed "s/ .*//g"`
adb -s ${SERIALNUMBER} wait-for-device
echo ""
echo "Serial number: ${SERIALNUMBER}"
echo "Number of start: ${COUNT}"
echo "Length of sleep between restarts: ${SLEEP_TIME}"
echo "Output directory: ${OUTPUTDIR}"
echo "Commission: ${COMISSION}"
echo "Test PPA: ${TEST_PPA}"
echo "PPA: ${PPA}"
echo ""


# Flash the device with rc-proposed
if [ ${COMISSION} == true ]; then
	device_provisioning
	unlock_screen
else
	# Check if the device is in writable mode
	if adb shell "echo ${PASSWORD}|sudo -S bash -c '[[ -f /userdata/.writable_image ]] &&echo && echo writable'" 2>&1 |grep -v password | grep -q writable ; then
		echo "The device is already writable"
		unlock_screen
		network
	else
		phablet-config -s ${SERIALNUMBER} writable-image -r ${PASSWORD} 2>&1 > /dev/null
		echo "Sleep after phablet-config";
		sleep_indicator 120
		echo -e "Clone the network "
		network
		adb -s ${SERIALNUMBER} shell "echo ${PASSWORD}|sudo -S reboot 2>&1|grep -v password"
		sleep_indicator 120
	fi
fi

# Install the ubuntu-app-launch-profiler on the device if needed
if adb shell "dpkg -l ubuntu-app-launch-profiler" 2>&1 | grep -q "no packages found" ; then
	echo "Installing ubuntu-app-launch-profiler on the device"
	if adb -s ${SERIALNUMBER} shell "echo ${PASSWORD}|sudo -S apt-cache search ubuntu-app-launch-profiler  2>/dev/null"|grep -q ubuntu-app-launch-profiler; then
		adb -s ${SERIALNUMBER} shell "echo ${PASSWORD}|sudo -S apt-get update 2>&1|grep -v password > /dev/null"
		adb -s ${SERIALNUMBER} shell "echo ${PASSWORD}|sudo -S apt-get install --yes --force-yes ubuntu-app-launch-profiler 2>&1 |grep -v password > /dev/null"
	else
		echo "ubuntu-app-launch-profiler is not available from the archives."
		echo "Please install it manually and press enter."
		read
	fi
else
	echo "The ubuntu-app-launch-profiler is installed on the device"
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

# Measure the application startup times with stock system
measure_app_startups

# Configure the PPA on the device if requested
if [[ ${TEST_PPA} == true ]]; then
	echo "Testing with ${PPA}"
	if adb -s ${SERIALNUMBER} shell "echo ${PASSWORD}|sudo -Sbash -c 'ls -l /etc/apt/sources.list.d/' 2>&1"|grep -q testing-ppa.list; then
		echo "Already set up"
	else
		echo -e "Set up with the PPA \e[31m${PPA}\e[0m"
		adb -s ${SERIALNUMBER} shell "echo ${PASSWORD}|sudo -S bash -c 'echo \"deb http://ppa.launchpad.net/${PPA}/${DISTRO} ${SERIES} main\" > /etc/apt/sources.list.d/testing-ppa.list' 2>&1|grep -v password > /dev/null"
		PPA=${PPA/\//-}
		adb -s ${SERIALNUMBER} shell "echo ${PASSWORD}|sudo -S bash -c 'echo -e \"Package: *\nPin: release o=LP-PPA-${PPA}\nPin-Priority: 1100\" > /etc/apt/preferences.d/silo.pref' 2>&1|grep -v password > /dev/null "
		adb -s ${SERIALNUMBER} shell "echo ${PASSWORD}|sudo -S apt-get update 2>&1|grep -v password > /dev/null"
		adb -s ${SERIALNUMBER} shell "echo ${PASSWORD}|sudo -S apt-get dist-upgrade --yes --force-yes 2>&1 |grep -v password > /dev/null"
		adb -s ${SERIALNUMBER} shell "echo ${PASSWORD}|sudo -S dpkg --configure -a 2>&1 |grep -v password > /dev/null"
		adb -s ${SERIALNUMBER} shell "echo ${PASSWORD}|sudo -S apt-get --force-yes -f install  2>&1 |grep -v password > /dev/null"
	        adb -s ${SERIALNUMBER} shell "echo ${PASSWORD}|sudo -S reboot 2>&1|grep -v password > /dev/null"
        	sleep_indicator 120
	fi
        unlock_screen	
	network
        sleep_indicator 10
	# Measure the application startup times with the PPA
	measure_app_startups
fi

