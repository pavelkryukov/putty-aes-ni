#!/bin/bash
# run.sh
#
# Run of unit tests
#
# @author Pavel Kryukov <kryukov@frtk.ru>
# @author Maxim Kuznetsov <maks.kuznetsov@gmail.com>
# @version 3.2
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

# Command for running SSHAESNI test
RUNAESNI_TEST="./sshaesni-test $SEEDS"
RUNAESNI_PERF="./sshaesni-perf $SEEDS"

# Make cpuid and tests
make cpuid sshaes-test sshaesni-test

####

echo -n "Checking for AES-NI support... "
if ./cpuid -q ;
then
    echo "found"
else
    echo "not found"
    echo -n "Checking for SDE... "
    
    if [ ! `which sde` ];
        then
            echo -e "not found"
            exit
        else
            echo "found `sde -version | grep 'Ver' | sed 's/\(.*\)Version\:\( *\)\(.*\)/\3/g'`"
            RUNAESNI_TEST="sde -no-avx -- "$RUNAESNI_TEST
            RUNAESNI_PERF="sde -no-avx -- "$RUNAESNI_PERF
        fi
fi

####

if [ -f "test-original-$SEEDS.txt" ];
then
    echo "Original trace is found, it will be reused"
else
    echo "No original trace file. It will be generated..."
    ./sshaes-test $SEEDS
    mv test-output.txt test-original-$SEEDS.txt
fi

echo "Generating aes-ni trace..."
$RUNAESNI_TEST

####

echo "MD5 check..."
original=$(md5sum test-original-$SEEDS.txt | cut -d ' ' -f 1)
changed=$(md5sum test-output.txt | cut -d ' ' -f 1)

if [ "$original" = "$changed" ];
then 
    echo "Tests passed!"
else
    echo "Tests not passed!"
    exit
fi

#####

if [ ! "$1" = "-p" ];
then
    exit
fi

make sshaes-perf sshaesni-perf
echo "Performance data generating ..."
if [ -f "perf-original.txt" ];
then
    echo "Original perf data is found, it will be reused"
else
    echo "No original perf file. It will be generated..."
    ./sshaes-perf
    mv perf-output.txt perf-original.txt
fi
echo "Generating aes-ni perf data..."
$RUNAESNI_PERF

echo "Sorting..."
sort perf-original.txt > perf-original.sorted
sort perf-output.txt > perf-output.sorted
mv perf-original.sorted perf-original.txt
mv perf-output.sorted perf-output.txt
