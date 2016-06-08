#!/bin/bash

#TO BE RUN FROM ROOT OF UITK SOURCE TREE
source export_qml_dir.sh

cd scrollbar_benchmarks || ( echo "Please run from the root of the source tree" && exit 1 )

ID=`bzr log --show-ids -r-1 | grep revision-id | cut -d '-' -f 3-4`
BENCH_PATH=`realpath .`
QMLBENCH_RESULT=$BENCH_PATH/results/$ID.qmlbench
TST_PERFORMANCE_RESULT=$BENCH_PATH/results/$ID.tst_performance

#../qmlbench/qmlbench ./qmlbench_benchmarks --json --fps-override 63 --id $ID > $QMLBENCH_RESULT

cd ../tests/unit/tst_performance && ../../../build_native/tests/unit/tst_performance/tst_performance -o $TST_PERFORMANCE_RESULT,csv -minimumtotal 5000

#Also works if you have multiple Scrollbar or ScrollbarCpp benchmarks (takes the avg in that case)
function parseResults {
	cat $1 | awk -F',' '{if ($2 ~ / Scrollbar / ) { sum1+=$5; cnt1+=1 } } {if ($2 ~ / ScrollbarCpp / ) { sum2+=$5; cnt2+=1 } } END {print sum1/cnt1, sum2/cnt2}'
}

function getResultsFromFile {
	#use TMP_ prefix to export results as global variables, hacky
	TMP_RESULTS=`parseResults $1`
	[[ $? -ne 0 ]] && echo "Results parsing failed! File $1" && exit 1
	TMP_QML_AVG=`echo $TMP_RESULTS | cut -d' ' -f1`
	TMP_CPP_AVG=`echo $TMP_RESULTS | cut -d' ' -f2`
	[ -z "$TMP_QML_AVG" -o -z "$TMP_CPP_AVG" ]  && echo "Empty results!" && exit 1
}

function compareResults {
	[ $# -ge 2 ] || ( echo "Not enough parameters. Please provide two revisions to compare." )
	[ -r "$1" ] || ( echo "$1 does not exist or is not readable"; exit 1 )
	[ -r "$2" ] || ( echo "$2 does not exist or is not readable"; exit 1 )

	getResultsFromFile $1
	FIRST_RESULTS=$TMP_RESULTS
	FIRST_QML_AVG=$TMP_QML_AVG
	FIRST_CPP_AVG=$TMP_CPP_AVG

        getResultsFromFile $2
        SECOND_RESULTS=$TMP_RESULTS
        SECOND_QML_AVG=$TMP_QML_AVG
        SECOND_CPP_AVG=$TMP_CPP_AVG

#triple slash for \n is needed because we want awk to print "\n" on the string, not the newline
	RESULTS_STRING+=`awk -v qml=$FIRST_QML_AVG -v cpp=$FIRST_CPP_AVG -v old_qml=$SECOND_QML_AVG -v old_cpp=$SECOND_CPP_AVG \
		-v rev1=\`basename $1\` -v rev2=\`basename $2\` \
		'BEGIN { print rev1, rev2, ((qml/old_qml)-1)*100"%", ((cpp/old_cpp)-1)*100"%", ((qml/cpp)-1)*100"%", "\\\n" }'`
}

RESULTS_STRING='CURRENT_REVISION OLD_REVISION QML_CHANGE CPP_CHANGE QML_VS_CPP\n'
REV_TO_COMPARE_TO=2
PREVIOUS_RESULTS_FILENAME=`ls -r $BENCH_PATH/results/*.tst_performance | awk -v rev=$REV_TO_COMPARE_TO 'NR==rev'`
while [ ! -z $PREVIOUS_RESULTS_FILENAME ]
do
	compareResults $TST_PERFORMANCE_RESULT $PREVIOUS_RESULTS_FILENAME
	((REV_TO_COMPARE_TO++))
	PREVIOUS_RESULTS_FILENAME=`ls -r $BENCH_PATH/results/*.tst_performance | awk -v rev=$REV_TO_COMPARE_TO 'NR==rev'`
done

#i.e. if the while did not execute, because no other file was found
if [ $REV_TO_COMPARE_TO -le 2 ]
then
	getResultsFromFile $TST_PERFORMANCE_RESULT
	echo "Nothing to compare to. QML $TMP_QML_AVG ms CPP ${TMP_CPP_AVG}ms"
else
	#-e so that it interprets \n
	#column -t to tabulize output
	echo -e $RESULTS_STRING | column -t
fi
