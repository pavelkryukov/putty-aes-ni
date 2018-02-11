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

if [ "$1" = "-p" ];
then
    echo "[check] Perfomance mode enabled"
    SEEDS="5"
else
    SEEDS="500"
fi

mkdir obj bin txt -p
make bin/aescpuid
make bin/shacpuid

echo -n "[check] AES-NI support... "
if ./bin/aescpuid -q ;
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
        fi
fi

make txt/test-original-aes-$SEEDS.txt txt/test-output-aes-$SEEDS.txt SDE=$SDE

original=$(md5sum txt/test-original-aes-$SEEDS.txt | cut -d ' ' -f 1)
changed=$(md5sum txt/test-output-aes-$SEEDS.txt | cut -d ' ' -f 1)

if [ "$original" = "$changed" ];
then 
    echo "********** Tests passed! **************"
else
    echo "********** Tests not passed! **************"
    exit 2
fi

echo -n "[check] SHA-NI support... "
if ./bin/shacpuid -q ;
then
    echo "found"
    SDE=""
else
    echo "not found"
    echo -n "[check] SDE... "
    
    if [ ! `which sde` ];
        then
            echo "found"
            exit 1
        else
            SDE="yes"
        fi
fi

make txt/test-original-sha-$SEEDS.txt txt/test-output-sha-$SEEDS.txt SDE=$SDE

original=$(md5sum txt/test-original-sha-$SEEDS.txt | cut -d ' ' -f 1)
changed=$(md5sum txt/test-output-sha-$SEEDS.txt | cut -d ' ' -f 1)

if [ "$original" = "$changed" ];
then 
    echo "********** Tests passed! **************"
else
    echo "********** Tests not passed! **************"
    exit 2
fi

if [ ! "$1" = "-p" ];
then
    exit
fi

make txt/perf-geomean-aes.txt SDE=$SDE && echo -n "Speedup in geomean is: " && cat txt/perf-geomean-aes.txt
