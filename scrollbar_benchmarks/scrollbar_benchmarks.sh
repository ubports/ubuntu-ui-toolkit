#!/bin/bash

#TO BE RUN FROM ROOT OF UITK SOURCE TREE
source export_qml_dir.sh

cd scrollbar_benchmarks || ( echo "Please run from the root of the source tree" && exit 1 )

BENCH_PATH=`realpath .`
ID=`bzr log --show-ids -r-1 | grep revision-id | cut -d '-' -f 3-4`
../qmlbench/qmlbench ./qmlbench_benchmarks --json --fps-override 63 --id $ID > $BENCH_PATH/results/$ID.result

cd ../tests/unit/tst_performance && ../../../build_native/tests/unit/tst_performance/tst_performance > $BENCH_PATH/results/$ID.result_tst_performance
