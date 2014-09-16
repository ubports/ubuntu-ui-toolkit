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
RTM=true
DISTRO="ubuntu-rtm" 
SERIES="14.09"
CHANNEL="ubuntu-touch/${DISTRO}/${SERIES}-proposed"
PWD="0000"
    
declare -a TEST_SUITE=(
    " -p reminders-app-autopilot reminders"
    " -p dialer-app-autopilot dialer_app"
    " -p messaging-app-autopilot messaging_app"
    " -p ubuntu-system-settings-autopilot ubuntu_system_settings"
    " -p ubuntu-html5-ui-toolkit-autopilot ubuntu_html5_ui_toolkit"
    " -p unity-webapps-qml-autopilot unity_webapps_qml"
    " filemanager"
    " dropping_letters_app"
    " sudoku_app"
    " -p ubuntu-ui-toolkit-autopilot ubuntuuitoolkit"
    " -p webbrowser-app-autopilot webbrowser_app"
    " shorts_app"
    " calendar_app"
    " music_app"
    " -p mediaplayer-app-autopilot mediaplayer_app"
    " ubuntu_terminal_app"
    " ubuntu_clock_app"
    " ubuntu_calculator_app"
    " ubuntu_weather_app"
    " -p address-book-app-autopilot address_book_app"
    " online_accounts_ui"
    " gallery_app"
    " -n unity8"
    " -p camera-app-autopilot camera_app"
)


UITK_PACKAGES="qtdeclarative5-ubuntu-ui-toolkit-plugin \
               ubuntu-ui-toolkit-autopilot \
               ubuntu-ui-toolkit-theme"

AP_PACKAGES="address-book-service-dummy \
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
             unity-webapps-qml-autopilot \
             ubuntu-system-settings-autopilot\
             ubuntu-html5-ui-toolkit-autopilot \
             ubuntu-system-settings-online-accounts-autopilot"

function network {
        if phablet-network -s ${SERIALNUMBER} 2>&1|grep -q Error; then
                echo "Reset and try again."
                reset -f
                phablet-network -s ${SERIALNUMBER} 2>&1
        fi
}

function reset {
     if [ ${RESET} == true  -o  x"$1" == x-f ]; then
        adb shell "echo 0000|sudo -S reboot 2>&1|grep -v password";sleep 5
        /usr/share/qtcreator/ubuntu/scripts/device_wait_for_shell ${SERIALNUMBER} > /dev/null
        sleep 60
        network
        phablet-config autopilot --dbus-probe enable 2>&1 > /dev/null
        adb shell powerd-cli display on |egrep -v "Display State requested, cookie is|Press ctrl-c to exit|not fully supported." &
        adb shell powerd-cli active |egrep -v "requested, cookie is|Press ctrl-c to exit|not fully supported." &
        sleep 60
        adb shell "sudo -u phablet -i gdbus call --session --dest com.canonical.UnityGreeter \
                                                 --object-path / \
                                                 --method com.canonical.UnityGreeter.HideGreeter|grep -v '\(\)'"
        adb shell "echo 0000|sudo -S dbus-send   --system --print-reply \
                                                 --dest=org.freedesktop.Accounts \
                                                 /org/freedesktop/Accounts/User32011 \
                                                 org.freedesktop.DBus.Properties.Set \
                                                 string:com.canonical.unity.AccountsService \
                                                 string:demo-edges variant:boolean:false 2>&1|grep -v password|egrep -v '\(\)|method return'"
    fi
}

function device_comission {
    adb -s ${SERIALNUMBER} wait-for-device
    # Avoid https://bugs.launchpad.net/gallery-app/+bug/1363190
    adb shell "echo 0000 |sudo -S rm -rf /userdata/user-data/phablet/.cache/com.ubuntu.gallery 2>&1|grep -v password"
    # flash the latest image
    echo -e "Flashing \e[31m${CHANNEL}\e[0m"
    ubuntu-device-flash --serial=${SERIALNUMBER} --channel=${CHANNEL} --wipe --developer-mode --password=0000 
    echo "Sleep for 5 minutes"
    sleep 300
    echo -e "Disable the intro wizard"
    phablet-config -s ${SERIALNUMBER}  welcome-wizard --disable
    echo -e "Disable the edge swiping lecture "
    phablet-config -s ${SERIALNUMBER} edges-intro --disable
    echo -e "Clone the network "
    network
    echo -e "phablet-click-test-setup  \e[31m${DISTRO} ${SERIES}\e[0m"
    phablet-click-test-setup --distribution=${DISTRO} --series=${SERIES} 2>&1 > /dev/null
    if [ ${PPA} == "archive"  ]; then
        echo "Set up with the archive image"
        phablet-config -s ${SERIALNUMBER} writable-image -r 0000 2>&1 > /dev/null
        network
    else
        if [[ "$PPA" =~ [^0-9]* ]]; then
            echo -e "Set up with the silo \e[31m${PPA}\e[0m"
            phablet-config -s ${SERIALNUMBER} writable-image -r 0000 2>&1 > /dev/null
            network
            adb shell "echo 0000|sudo -S  bash -c 'echo \"deb http://ppa.launchpad.net/ci-train-ppa-service/landing-${PPA}/${DISTRO} ${SERIES} main\" > /etc/apt/sources.list.d/silo-${PPA}.list'"
            adb shell "echo 0000|sudo -S apt-get update 2>&1|grep -v password > /dev/null"
        else
            echo  -e "Set up with the PPA \e[31m${PPA}\e[0m"
            phablet-config -s ${SERIALNUMBER} writable-image -r 0000 -p ${PPA} 2>&1 > /dev/null
        fi
    fi
    adb -s ${SERIALNUMBER} shell rm -rf /home/phablet/autopilot/ubuntuuitoolkit
    UITK_VERSION=`adb -s ${SERIALNUMBER} shell "stty cols 250; dpkg -l"|grep qtdeclarative5-ubuntu-ui-toolkit-plugin|awk '{print $3}'`
    echo -e "Original UITK version:\t\e[31m${UITK_VERSION}\e[0m"
    echo "Updating APT";adb shell "echo 0000 |sudo -S apt-get update  2>&1|grep -v password > /dev/null"
    echo "Install the UITK packages"
    adb -s ${SERIALNUMBER} shell "echo 0000|sudo -S apt-get install --yes --force-yes ${UITK_PACKAGES} 2>&1 |grep -v password > /dev/null"
    UITK_VERSION=`adb -s ${SERIALNUMBER} shell "stty cols 250; dpkg -l"|grep qtdeclarative5-ubuntu-ui-toolkit-plugin|awk '{print $3}'`
    echo -e "New UITK version:\t\e[31m${UITK_VERSION}\e[0m"
    # Update APT
    adb -s ${SERIALNUMBER} shell "echo 0000 |sudo -S apt-get update 2>&1|grep -v password > /dev/null"
    # Install the autopilot tests for the apps
    adb -s ${SERIALNUMBER} shell "echo 0000 |sudo -S apt-get install --yes --force-yes ${AP_PACKAGES} 2>&1 | grep -v password > /dev/null"
}

while getopts ":hrcintdus:o:p:f:" opt; do
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
        f)
            FILTER=$OPTARG
            ;;
        c)
            COMISSION=true
            ;;
        n)
            DONOTRUNTESTS=true
            ;;
        u)
            RTM=false
            CHANNEL="ubuntu-touch/utopic-proposed"
            DISTRO="ubuntu"
            SERIES="utopic"
            ;;
        h)
            echo "Usage: uitk_test_plan.sh -s [serial number] -m -c"
            echo " -r : Reset after each tests. Default: ${RESET}"
            echo " -s : Use the device with the given serial number. Default: ${SERIALNUMBER}"
            echo " -c : Comission the device with the ${PPA} enabled"
            echo " -n : Do not run the test set. Default ${DONOTRUNTESTS}"
            echo " -o : Output directory. Default $OUTPUTDIR"
            echo " -p : Source PPA for the UITK. Default $PPA. Use -p archive to test stock image or -p [0-9]* to set a silo."
            echo " -f : Filter for the test suite. Default $FILTER"
            echo " -u : Provision the Development release of Ubuntu. Default is RTM."
            exit
            ;;
        :)
            echo "Option -$OPTARG requires an argument." >&2
            exit
    esac
done

# Use the first available device for testing
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

# Reset the device for testing
reset -f

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
            reset -f
            LOGFILE="$OUTPUTDIR/${LOGFILENAME}-${APPNAME}-2.tests"
            COMMAND="phablet-test-run -s ${SERIALNUMBER} $TEST_SET >> ${LOGFILE}"
                echo "<<<=== ${APPNAME} 2 ===>>>" >> ${LOGFILE}
            eval ${COMMAND}
            egrep "<<<===|Ran|OK|FAILED" ${LOGFILE}
                if grep -q "FAILED" ${LOGFILE}; then
                        reset -f
                        LOGFILE="$OUTPUTDIR/${LOGFILENAME}-${APPNAME}-3.tests"
                        COMMAND="phablet-test-run -s ${SERIALNUMBER} $TEST_SET >> ${LOGFILE}"
                    echo "<<<=== ${APPNAME} 3 ===>>>" >> ${LOGFILE}
                        eval ${COMMAND}
                egrep "<<<===|Ran|OK|FAILED|FAIL|ERROR:" ${LOGFILE}
            fi
        fi
    fi
done
