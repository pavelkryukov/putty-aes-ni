#!/bin/bash
# run.sh
#
# Run of unit tests
#
# @author Pavel Kryukov <kryukov@frtk.ru>
# @author Maxim Kuznetsov <maks.kuznetsov@gmail.com>
# @version 4.0
#
# For Putty AES NI project
# http://putty-aes-ni.googlecode.com/

if [ "$2" = "-p" ];
then
    echo "[check] Perfomance mode enabled"
    SEEDS="5"
else
    SEEDS="100"
fi

if [ `uname -m` = x86_64 ];
then
    SDE_BIN=sde64
else
    SDE_BIN=sde
fi

mkdir obj bin txt -p
make bin/${1}cpuid

echo -n "[check] $1-NI support... "
if ./bin/${1}cpuid -q ;
then
    echo "found"
    SDE=""
else
    echo "not found"
    echo -n "[check] SDE... "
    
    if [ ! `which sde` ];
    then
        echo "not found"
        exit 1
    else
        echo "found"
        SDE="yes"
        $SDE_BIN -- ./bin/${1}cpuid;
    fi
fi

make txt/test-original-$1-$SEEDS.txt txt/test-output-$1-$SEEDS.txt SDE=$SDE SDE_BIN=$SDE_BIN

if [ ! -f txt/test-original-$1-$SEEDS.txt ]; then
    echo "Original output is not generated"
    echo "********** Tests not passed! **************"
    exit 2
fi

if [ ! -f txt/test-output-$1-$SEEDS.txt ]; then
    echo "Optimized output is not generated"
    echo "********** Tests not passed! **************"
    exit 2
fi

original=$(md5sum txt/test-original-$1-$SEEDS.txt | cut -d ' ' -f 1)
changed=$(md5sum txt/test-output-$1-$SEEDS.txt | cut -d ' ' -f 1)

if [ "$original" = "$changed" ];
then 
    echo "********** Tests passed! **************"
else
    echo "Mismatch between software and optimized implementations"
    echo "********** Tests not passed! **************"
    exit 2
fi

if [ ! "$2" = "-p" ];
then
    exit
fi

make txt/perf-geomean-$1.txt SDE=$SDE && echo -n "Speedup in geomean is: " && cat txt/perf-geomean-$1.txt
