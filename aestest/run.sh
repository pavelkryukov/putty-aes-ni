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
    echo "Perfomance mode enabled"
    SEEDS="5"
else
    SEEDS="50"
fi

mkdir obj bin txt -p
make bin/cpuid

echo -n "Checking for AES-NI support... "
if ./bin/cpuid -q ;
then
    echo "found"
    SDE=""
else
    echo "not found"
    echo -n "Checking for SDE... "
    
    if [ ! `which sde` ];
        then
            echo -e "not found"
            exit
        else
            echo "found `sde -version | grep 'Ver' | sed 's/\(.*\)Version\:\( *\)\(.*\)/\3/g'`"
            SDE="yes"
        fi
fi

make txt/test-original-$SEEDS.txt txt/test-output-$SEEDS.txt SDE=$SDE

echo "MD5 check..."
original=$(md5sum txt/test-original-$SEEDS.txt | cut -d ' ' -f 1)
changed=$(md5sum txt/test-output-$SEEDS.txt | cut -d ' ' -f 1)

if [ "$original" = "$changed" ];
then 
    echo "********** Tests passed! **************"
else
    echo "********** Tests not passed! **************"
    exit
fi

if [ ! "$1" = "-p" ];
then
    exit
fi

make txt/perf-original.sorted.txt txt/perf-output.sorted.txt SDE=$SDE
