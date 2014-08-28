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

LAZY=true
SERIALNUMBER=086e443edf51b915
RESET=false
COMISSION=false
DONOTRUNTESTS=false
PPA="ppa:ubuntu-sdk-team/staging"
TIMESTAMP=`date +"%Y_%m_%d-%H_%M_%S"`
LOGFILENAME="ap-${TIMESTAMP}"
OUTPUTDIR=$HOME
FILTER=.*

declare -a TEST_SUITE=(
" dropping_letters_app"
" sudoku_app"
" -p ubuntu-ui-toolkit-autopilot ubuntuuitoolkit"
" gallery_app"
" -p webbrowser-app-autopilot webbrowser_app"
" -n unity8"
" shorts_app"
" calendar_app"
" music_app"
" -p mediaplayer-app-autopilot mediaplayer_app"
" ubuntu_terminal_app"
" ubuntu_clock_app"
" ubuntu_calculator_app"
" ubuntu_weather_app"
" filemanager"
" -p reminders-app-autopilot reminders"
" -p address-book-app-autopilot address_book_app"
" -p dialer-app-autopilot dialer_app"
" -p messaging-app-autopilot messaging_app"
" -p ubuntu-system-settings-autopilot ubuntu_system_settings"
" -p ubuntu-html5-ui-toolkit-autopilot ubuntu_html5_ui_toolkit"
" -p unity-webapps-qml-autopilot unity_webapps_qml"
" online_accounts_ui"
" -p camera-app-autopilot camera_app"

)

function reset {
	if [ ${RESET} == true  ]; then
		adb shell reboot;sleep 5
		/usr/share/qtcreator/ubuntu/scripts/device_wait_for_shell ${SERIALNUMBER} > /dev/null
		phablet-config autopilot --dbus-probe enable
		adb shell powerd-cli display on |egrep -v "Display State requested, cookie is|Press ctrl-c to exit" &
		sleep 60
	fi
}

function device_comission {
	# flash the latest image
	ubuntu-device-flash --serial=${SERIALNUMBER} --channel=ubuntu-touch/utopic-proposed --wipe 
	adb -s ${SERIALNUMBER} wait-for-device
	echo "Disable the intro wizard";phablet-config -s ${SERIALNUMBER} welcome-wizard --disable
	sleep 60
	echo "Disable the edge swiping lecture";phablet-config -s ${SERIALNUMBER} edges-intro --disable
	echo "clone the network";phablet-network -s ${SERIALNUMBER}
	sleep 60
	reset
	echo "phablet-click-test-setup";phablet-click-test-setup -s ${SERIALNUMBER}
	reset
	phablet-config -s ${SERIALNUMBER} writable-image --ppa ${PPA}
	reset
	adb -s ${SERIALNUMBER} shell rm -rf /home/phablet/autopilot/ubuntuuitoolkit
	UITK_VERSION=`adb -s ${SERIALNUMBER} shell "stty cols 250; dpkg -l"|grep qtdeclarative5-ubuntu-ui-toolkit-plugin|awk '{print $3}'`
	echo "Original UITK version:\t${UITK_VERSION}"
	echo "update";adb shell apt-get update
	echo "install the UITK from the silo"
	adb -s ${SERIALNUMBER} shell apt-get install -y qtdeclarative5-ubuntu-ui-toolkit-plugin \
							ubuntu-ui-toolkit-autopilot \
							ubuntu-ui-toolkit-theme
	UITK_VERSION=`adb -s ${SERIALNUMBER} shell "stty cols 250; dpkg -l"|grep qtdeclarative5-ubuntu-ui-toolkit-plugin|awk '{print $3}'`
	echo "New UITK version:\t${UITK_VERSION}"
	# update
	adb -s ${SERIALNUMBER} shell apt-get update
	# install the autopilot tests 
	adb -s ${SERIALNUMBER} shell apt-get install -y	address-book-service-dummy \
                                                        python3-lxml \
							ubuntu-ui-toolkit-autopilot \
							gallery-app-autopilot \
							reminders-app-autopilot \
							address-book-app-autopilot \
							messaging-app-autopilot \
							dialer-app-autopilot \
							camera-app-autopilot \
							webbrowser-app-autopilot \
							mediaplayer-app-autopilot \
							unity8-autopilot \
							ubuntu-system-settings-online-accounts-autopilot
}

while getopts ":hrcnts:o:p:f:" opt; do
	case $opt in
		m)
			RESET=true
			;;
		s)
			SERIALNUMBER=$OPTARG
			LAZY=false
			;;
		o)
			if [ -d "$OPTARG" ]; then
				OUTPUTDIR=$OPTARG
			else
				echo "$OPTARG does not exist. Using default $OUTPUTDIR"
			fi
			;;
		p)
			PPA=$OPTARG
			;;
		f)
			FILTER=$OPTARG
			;;
		c)
			COMISSION=true
			;;
		n)
			DONOTRUNTESTS=true
			;;
		h)
			echo "Usage: uitk_test_plan.sh -s [serial number] -m -c"
			echo " -r : Reset after each tests. Default: ${RESET}"
			echo " -s : Use the device with the given serial number. Default: ${SERIALNUMBER}"
			echo " -c : Comission the device with the ${PPA} enabled"
			echo " -n : Do not run the test set. Default ${DONOTRUNTESTS}"
			echo " -o : Output directory. Default $OUTPUTDIR"
			echo " -p : Source PPA for the UITK. Default $PPA"
			echo " -f : Filter for the test suite. Default $FILTER"
			exit
			;;
		:)
			echo "Option -$OPTARG requires an argument." >&2
			exit
	esac
done

if [ ${LAZY} == true ]; then
	echo "Waiting for a device"
	adb wait-for-device
	SERIALNUMBER=`adb devices -l | grep -Ev "List of devices attached" | grep -Ev "emulator-" | sed "/^$/d"|sed "s/ .*//g"`
	echo "Tests will be run on the device with ${SERIALNUMBER} serial number."
else

	echo "Waiting for the device with ${SERIALNUMBER} serial number."
	adb -s ${SERIALNUMBER} wait-for-device
fi

# Check if the device need to be flashed and set up for testing
if [ ${COMISSION} == true ]; then
	device_comission
fi

# Check if the job is only comissioning the device
if [ ${DONOTRUNTESTS} == true ]; then
        exit
fi

if [ ${RESET} == false ]; then
	echo "Reset the device for testing."
        RESET=true
        reset
        RESET=false
else
        reset
fi

# Run the test suite
for TEST_SET in "${TEST_SUITE[@]}"
do
	if [[ ${TEST_SET} =~ ${FILTER} ]]; then
		APPNAME=${TEST_SET##* }
		LOGFILE="$OUTPUTDIR/${LOGFILENAME}-${APPNAME}-1.tests"
		COMMAND="phablet-test-run -s ${SERIALNUMBER} $TEST_SET >> ${LOGFILE}"
		echo "<<<=== ${APPNAME} 1 ===>>>" >> ${LOGFILE}
		reset
		eval ${COMMAND}
		egrep "<<<===|Ran|OK|FAILED" ${LOGFILE}
		# check if the tests were successful and re-run after a reset
		if grep -q "FAILED" ${LOGFILE}; then 
		        if [ ${RESET} == false  ]; then
				RESET=true
				reset
				RESET=false
			else
				reset
			fi
			LOGFILE="$OUTPUTDIR/${LOGFILENAME}-${APPNAME}-2.tests"
			COMMAND="phablet-test-run -s ${SERIALNUMBER} $TEST_SET >> ${LOGFILE}"
	        	echo "<<<=== ${APPNAME} 2 ===>>>" >> ${LOGFILE}
			eval ${COMMAND}
			egrep "<<<===|Ran|OK|FAILED" ${LOGFILE}
	        	if grep -q "FAILED" ${LOGFILE}; then
		                if [ ${RESET} == false  ]; then
        		                RESET=true
                		        reset
                        		RESET=false
		                else
        		                reset
                		fi
		                LOGFILE="$OUTPUTDIR/${LOGFILENAME}-${APPNAME}-3.tests"
        		        COMMAND="phablet-test-run -s ${SERIALNUMBER} $TEST_SET >> ${LOGFILE}"
			        echo "<<<=== ${APPNAME} 3 ===>>>" >> ${LOGFILE}
		                eval ${COMMAND}
				egrep "<<<===|Ran|OK|FAILED|FAIL|ERROR:" ${LOGFILE}
			fi

		fi
	fi
done
