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
PPA="ubuntu-sdk-team/staging"
TIMESTAMP=`date +"%Y_%m_%d-%H_%M_%S"`
DATESTAMP=`date +"%Y_%m_%d"`
LOGFILENAME="ap-${TIMESTAMP}"
OUTPUTDIR=$HOME
FILTER=.*
RTM=true
REVISION=105
DISTRO="ubuntu" 
SERIES="vivid"
CHANNEL="ubuntu-touch/rc-proposed/${DISTRO}"
PASSWORD="0000"
BOOTTIME=500
ONLYCOMPARE=false
DISTUPGRADE=false
BOOTSTRAP=false
UNLOCK_ONLY=false

declare -a TEST_SUITE=(
    " -p ubuntu-ui-toolkit-autopilot ubuntuuitoolkit"
    " -p webbrowser-app-autopilot webbrowser_app"
    " -p address-book-app-autopilot address_book_app" 
    " sudoku_app"
    " online_accounts_ui"
# comment out if calculator AP:s broken
    " ubuntu_calculator_app"
    " -p mediaplayer-app-autopilot mediaplayer_app"
    " dropping_letters_app"
    " shorts_app"
    " ubuntu_weather_app"
    " -p ubuntu-system-settings-autopilot ubuntu_system_settings"
    " music_app"
    " gallery_app"
# comment out if filemanager AP:s broken
#   " filemanager"
#    " ubuntu_terminal_app"
    " -n unity8"
    " ubuntu_clock_app"
#    " -p dialer-app-autopilot dialer_app"
#    " -p reminders-app-autopilot reminders"
#    " -p messaging-app-autopilot messaging_app"
)


UITK_PACKAGES="qtdeclarative5-ubuntu-ui-toolkit-plugin \
               ubuntu-ui-toolkit-autopilot \
               ubuntu-ui-toolkit-theme"
#UITK_PACKAGES="unity8 unity8-common unity8-private"
#UITK_PACKAGES="qtmir-android qtdeclarative5-qtmir-plugin"


AP_PACKAGES="address-book-service-dummy \
             python3-lxml \
             ubuntu-ui-toolkit-autopilot \
             gallery-app-autopilot \
             reminders-app-autopilot \
             address-book-app-autopilot \
#             messaging-app-autopilot \
             unity8-autopilot \
             dialer-app-autopilot \
             camera-app-autopilot \
             webbrowser-app-autopilot \
             mediaplayer-app-autopilot \
             unity-webapps-qml-autopilot \
             ubuntu-system-settings-autopilot\
             ubuntu-html5-ui-toolkit-autopilot \
             ubuntu-system-settings-online-accounts-autopilot"
#             messaging-app-autopilot \

fatal_failure () {
    echo -e "\e[31mFailed operation:\e[0m $1"
    exit
}



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

function network {
        if phablet-network -s ${SERIALNUMBER} 2>&1|grep -q Error; then
                echo "Reset and try again."
                reset -f
                phablet-network -s ${SERIALNUMBER} 2>&1
        fi
}

function reset {
     if [ ${RESET} == true  -o  x"$1" == x-f ]; then
	if [ ${UNLOCK_ONLY} == false ]; then
            adb -s ${SERIALNUMBER} shell "echo ${PASSWORD}|sudo -S reboot 2>&1|grep -v password"
             sleep_indicator 120
             /usr/share/qtcreator/ubuntu/scripts/device_wait_for_shell ${SERIALNUMBER} > /dev/null
             sleep_indicator 10
             network
        fi
        phablet-config -s ${SERIALNUMBER} autopilot --dbus-probe enable 2>&1 > /dev/null
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
    fi
}

function device_comission {
    if [ ${BOOTSTRAP} == true ]; then
        # bootstrap the device with the latest image
	ubuntu-device-flash touch --channel=${CHANNEL} --wipe --bootstrap --developer-mode --password=0000
    else
        adb -s ${SERIALNUMBER} wait-for-device
        # Avoid https://bugs.launchpad.net/gallery-app/+bug/1363190
        adb -s ${SERIALNUMBER} shell "echo ${PASSWORD} |sudo -S rm -rf /userdata/user-data/phablet/.cache/com.ubuntu.gallery 2>&1|grep -v password"
        # flash the latest image
        echo -e "Flashing \e[31m${CHANNEL}\e[0m"
        ubuntu-device-flash touch --serial=${SERIALNUMBER} --channel=${CHANNEL} --wipe --developer-mode --password=${PASSWORD}
    fi
    sleep_indicator ${BOOTTIME}
    echo -e "Disable the intro wizard"
    phablet-config -s ${SERIALNUMBER}  welcome-wizard --disable
    echo -e "Disable the edge swiping lecture "
    phablet-config -s ${SERIALNUMBER} edges-intro --disable
    echo -e "Clone the network "
    network
    adb -s ${SERIALNUMBER} shell "echo ${PASSWORD}|sudo -S reboot 2>&1|grep -v password"
    sleep_indicator 120
    # Required for at least rtm-14.09/mako, phablet-click-test-setup fails otherwise and we don't need terminal
    adb -s ${SERIALNUMBER} shell "echo ${PASSWORD}|sudo -S click unregister com.ubuntu.terminal 2>&1|grep -v password"
    # Enable if calculator AP:s broken, to prevent phablet-click-test-setup trying to check out its tests.
    #adb -s ${SERIALNUMBER} shell "echo ${PASSWORD}|sudo -S click unregister com.ubuntu.calculator 2>&1|grep -v password"
    echo -e "phablet-click-test-setup  \e[31m${DISTRO} ${SERIES}\e[0m"
    phablet-click-test-setup -s ${SERIALNUMBER} --distribution=${DISTRO} --series=${SERIES} 2>&1 || fatal_failure "phablet-click-test-setup has failed" 
    echo "Sleep after phablet-click-test-setup";
    sleep_indicator 120
    if [ ${PPA} == "archive"  ]; then
        echo "Set up with the archive image"
        phablet-config -s ${SERIALNUMBER} writable-image -r ${PASSWORD} 2>&1 > /dev/null
        echo "Sleep after phablet-config";
        sleep_indicator 120
        network
    else
        if [[ "$PPA" =~ ^[0-9]{3}$ ]]; then
            echo -e "Set up with the silo \e[31m${PPA}\e[0m"
            phablet-config -s ${SERIALNUMBER} writable-image -r ${PASSWORD} 2>&1 > /dev/null
	    sleep_indicator 120
            network
            # TODO: hide the sudo output
            adb -s ${SERIALNUMBER} shell "echo ${PASSWORD}|sudo -S  bash -c 'echo \"deb http://ppa.launchpad.net/ci-train-ppa-service/landing-${PPA}/${DISTRO} ${SERIES} main\" > /etc/apt/sources.list.d/silo-${PPA}.list'  2>&1|grep -v password > /dev/null "
            adb -s ${SERIALNUMBER} shell "echo ${PASSWORD}|sudo -S apt-get update 2>&1|grep -v password > /dev/null"
        else
            echo  -e "Set up with the PPA \e[31m${PPA}\e[0m"
            phablet-config -s ${SERIALNUMBER} writable-image -r ${PASSWORD}  2>&1 > /dev/null
	    sleep_indicator 120
            network
            adb -s ${SERIALNUMBER} shell "echo ${PASSWORD}|sudo -S  bash -c 'echo \"deb http://ppa.launchpad.net/${PPA}/${DISTRO} ${SERIES} main\" > /etc/apt/sources.list.d/testing-ppa.list' 2>&1|grep -v password > /dev/null"
            adb -s ${SERIALNUMBER} shell "echo ${PASSWORD}|sudo -S apt-get update 2>&1|grep -v password > /dev/null"

        fi
    fi
    adb -s ${SERIALNUMBER} shell rm -rf /home/phablet/autopilot/ubuntuuitoolkit
    UITK_VERSION=`adb -s ${SERIALNUMBER} shell "stty cols 250; dpkg -l"|grep qtdeclarative5-ubuntu-ui-toolkit-plugin|awk '{print $3}'`
    echo -e "Original UITK version:\t\e[31m${UITK_VERSION}\e[0m"
    echo "Updating APT";
    adb -s ${SERIALNUMBER} shell "echo ${PASSWORD} |sudo -S apt-get update  2>&1|grep -v password > /dev/null"
    if [ ${DISTUPGRADE} == true ]; then
        echo "Install all the upgraded packages with dist-upgrade"
        adb -s ${SERIALNUMBER} shell "echo ${PASSWORD}|sudo -S apt-get dist-upgrade --yes --force-yes 2>&1 |grep -v password > /dev/null"
        adb -s ${SERIALNUMBER} shell "echo ${PASSWORD}|sudo -S dpkg --configure -a 2>&1 |grep -v password > /dev/null"
    else
        echo "Install the UITK packages"
        adb -s ${SERIALNUMBER} shell "echo ${PASSWORD}|sudo -S apt-get install --yes --force-yes ${UITK_PACKAGES} 2>&1 |grep -v password > /dev/null"
    fi
    UITK_VERSION=`adb -s ${SERIALNUMBER} shell "stty cols 250; dpkg -l"|grep qtdeclarative5-ubuntu-ui-toolkit-plugin|awk '{print $3}'`
    echo -e "New UITK version:\t\e[31m${UITK_VERSION}\e[0m"
    # Update APT
    adb -s ${SERIALNUMBER} shell "echo ${PASSWORD} |sudo -S apt-get update 2>&1|grep -v password > /dev/null"
    # Install the autopilot tests for the apps
    adb -s ${SERIALNUMBER} shell "echo ${PASSWORD} |sudo -S apt-get install --yes --force-yes ${AP_PACKAGES} 2>&1 | grep -v password > /dev/null"
    echo "Cleaning QML cache for the next boot"
    adb -s ${SERIALNUMBER} shell "rm -rf ~/.cache/QML/Apps/"
}

function compare_results {
    sed -i 's/\r//g' *tests
    for RESULT_FILE in *${PPA}.tests;
    do
        echo "$RESULT_FILE" >> ${MAINFILE};
        LAST_LINE=`tail -1 $RESULT_FILE`
        if [[ "$LAST_LINE" =~ OK || "$LAST_LINE" =~ FAILED ]]; then
	    echo -e  "\t"`tail -2 $RESULT_FILE` >> ${MAINFILE}
	else
	    echo -e "\tBroken tests"  >> ${MAINFILE}
        fi
        egrep -v "NO TAGS DATABASE" $RESULT_FILE |egrep "^ERROR:|^FAIL:" | while read -r FAILED ; 
        do
            echo -e "\tFailed with ${PPA} - $FAILED"  >> ${MAINFILE}
            if grep --quiet "$FAILED" *archive.tests; then
                echo -e "\tSame on archive"  >> ${MAINFILE}
            else
                echo -e "\tPossible regression"  >> ${MAINFILE}
            fi
        done
    done
}

while getopts ":hrcintduslwbv:o:p:f:a:" opt; do
    case $opt in
        r)
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
	a)
	    START_FROM=$OPTARG
            ;;
        f)
            FILTER=$OPTARG
            ;;
        v)
            REVISION=$OPTARG
            ;;
        c)
            COMISSION=true
            ;;
        d)
            ONLYCOMPARE=true
            ;;
        n)
            DONOTRUNTESTS=true
            ;;
        u)
            RTM=false
            CHANNEL="ubuntu-touch/devel-proposed/ubuntu"
            DISTRO="ubuntu"
            SERIES="wily"
            ;;
        w)
            DISTUPGRADE=true
            ;;
        b)
	   BOOTSTRAP=true
	   ;;
        l)
           UNLOCK_ONLY=true
           ;;
        h)
            echo "Usage: uitk_test_plan.sh -s [serial number] -m -c"
            echo -e "\t-r : Reset after each tests. Default: ${RESET}"
            echo -e "\t-s : Use the device with the given serial number. Default: ${SERIALNUMBER}"
            echo -e "\t-c : Comission the device with the ${PPA} enabled"
            echo -e "\t-d : Only compare the -p ${PPA} results with the archive test results. Default: ${ONLYCOMPARE}"
            echo -e "\t-n : Do not run the test set. Default ${DONOTRUNTESTS}"
            echo -e "\t-o : Output directory. Default $OUTPUTDIR"
            echo -e "\t-p : Source PPA for the UITK. Default $PPA. Use -p archive to test stock image or -p [0-9]* to set a silo."
            echo -e "\t-f : Filter for the test suite. Default $FILTER"
            echo -e "\t-a : Start the test suite from the given test."
            echo -e "\t-u : Provision the Development release of Ubuntu, Wily. Default is vivid-overlay (formerly RTM)."
            echo -e "\t-w : dist-upgrade to the whole PPA instead of just Ubuntu UI Toolkit. Default is only UITK."
            echo -e "\t-b : Bootstrap the device with the ${PPA} enabled."
            echo ""
            echo "By default tihe uitk_test_plan.sh flashes the latest vivid-overlay image on the device, installs the click application"
            echo "tests, configures the ppa:ubuntu-sdk-team/staging PPA, installs the UITK from the PPA and executes the test plan."
            echo ""
            echo "Validate the staging branch of the UITK against the vivid-overlay image"
            echo -e "\t$ ./uitk_test_plan.sh -c"
            echo ""
            echo "Validate the UITK from a CI silo on an vivid-overlay image"
            echo -e "\t$ ./uitk_test_plan.sh -c -p 001"
            echo ""
            echo "Validate the UITK from teh archive on an vivid-overlay image"
            echo -e "\t$ ./uitk_test_plan.sh -c -p archive"
            echo ""
            echo "Validate the UITK from a specific CI silo on an Ubuntu Wily image"
            echo -e "\t$ ./uitk_test_plan.sh -c -p 001 -u"
            echo ""
            echo "Provision the device for manual testing with the latest vivid-overlay image"
            echo -e "\t$ ./uitk_test_plan.sh -c -p archive -n"
            echo ""
            echo "Provision the device for manual testing with the latest Ubuntu Wily image"
            echo -e "\t$ ./uitk_test_plan.sh -c -p archive -u -n"
            echo ""
            echo "Run the test plan on an already provisioned device"
            echo -e "\t$ ./uitk_test_plan.sh"
            echo ""
            echo "Run only the UITK tests on an already provisioned device"
            echo -e "\t$ ./uitk_test_plan.sh -f ubuntuuitoolkit"
            echo ""
            echo "Compare the archive test results with the silo 001 results"
            echo -e "\t$ ./uitk_test_plan.sh -d -p 001"
	    exit
            ;;
        :)
            echo "Option -$OPTARG requires an argument." >&2
            exit
    esac
done

MAINFILE=MAIN-ap-${DATESTAMP}-${SERIES}-${PPA}
MAINFILE=${MAINFILE/\//_}

if [ ${ONLYCOMPARE} == true ]; then
   echo "Comparing results with the archive tests" > $MAINFILE
   PPA=${PPA/\//_}
   compare_results
   egrep "regression" -B1 ${MAINFILE} |grep Failed|sort|uniq -c|grep " 2"|sed 's/2//g'
   exit
fi

if [ ${BOOTSTRAP} == false ]; then
    # Use the first available device for testing
    if [ ${LAZY} == true ]; then
        echo "Waiting for a device"
        adb wait-for-device
        SERIALNUMBER=`adb devices -l | grep -Ev "List of devices attached" | grep -Ev "emulator-" | sed "/^$/d"|sed "s/ .*//g"`
    else
        echo "Waiting for the device with ${SERIALNUMBER} serial number."
        adb -s ${SERIALNUMBER} wait-for-device
    fi
fi

echo "*** Settings ***"
echo ""
echo "Serial number: ${SERIALNUMBER}"
echo "Output directory: ${OUTPUTDIR}"
echo "PPA: $PPA"
echo "Filter: ${FILTER}"
echo "Start with: ${START_FROM}"
echo ""
echo "Unlock only: ${UNLOCK_ONLY}"
echo "Reset: ${RESET}"
echo "Commission: ${COMISSION}"
echo "Bootstrap: ${BOOTSTRAP}"
echo "Only compare: ${ONLYCOMPARE}"
echo "Do not run tests: ${DONOTRUNTESTS}"
echo "RTM: ${RTM}"
echo "Dist-upgrade: ${DISTUPGRADE}"
echo "Main logs: ${MAINFILE}"
echo "*** Starting ***"
echo ""


if [ ${UNLOCK_ONLY} == true ]; then
   reset -f
   exit
fi
# Check if the device need to be flashed and set up for testing
if [ ${COMISSION} == true ]; then
    device_comission
    egrep "regression" -B1 ${MAINFILE} |grep Failed|sort|uniq -c|grep " 2"
fi

# Fix the PPA string as it is used in log file names
PPA=${PPA/\//_}

# Check if the job is only comissioning the device
if [ ${DONOTRUNTESTS} != true ]; then
    # Reset the device for testing
    reset -f
	
    # Run the test suite
    for TEST_SET in "${TEST_SUITE[@]}"
    do
	if [[ ! -z $START_FROM ]]; then
            if [[ ! ${TEST_SET} =~ ${START_FROM} ]]; then
        	continue
	     else
		unset START_FROM
	     fi
	fi
        if [[ ${TEST_SET} =~ ${FILTER} ]]; then
            APPNAME=${TEST_SET##* }
            LOGFILE="$OUTPUTDIR/${LOGFILENAME}-${APPNAME}-1-${PPA}.tests"
            COMMAND="phablet-test-run -r ${PASSWORD} -s ${SERIALNUMBER} $TEST_SET >> ${LOGFILE}"
            echo "<<<=== ${APPNAME} 1 ===>>>" >> ${LOGFILE}
            reset
            eval ${COMMAND}
             egrep "<<<===|Ran|OK|FAILED" ${LOGFILE}
            # check if the tests were successful and re-run after a reset
            if (tail -1 ${LOGFILE}|grep -q OK) ;then 
		echo -e "OK - ${LOGFILE}";
          else 
                echo -e "\e[31mFailed\e[0m - ${LOGFILE}";
                reset -f
                LOGFILE="$OUTPUTDIR/${LOGFILENAME}-${APPNAME}-2-${PPA}.tests"
                COMMAND="phablet-test-run -r ${PASSWORD} -s ${SERIALNUMBER} $TEST_SET >> ${LOGFILE}"
                echo "<<<=== ${APPNAME} 2 ===>>>" >> ${LOGFILE}
                eval ${COMMAND}
                egrep "<<<===|Ran|OK|FAILED" ${LOGFILE}
                if (tail -1 ${LOGFILE}|grep -q OK) ;then
                    echo -e "OK - ${LOGFILE}";
                else 
                    echo -e "\e[31mFailed\e[0m - ${LOGFILE}";
                fi
            fi
        fi
    done
fi

if [ ${PPA} != "archive"  ]; then 
   echo "Comparing results with the archive tests"  > $MAINFILE
   compare_results
fi
