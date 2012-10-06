#!/bin/bash
# run.sh
#
# Run of unit tests
#
# @author Pavel Kryukov <kryukov@frtk.ru>
# @author Maxim Kuznetsov <maks.kuznetsov@gmail.com>
# @version 3.0
#
# For Putty AES NI project
# http://putty-aes-ni.googlecode.com/

SSHAES_TEST="sshaes-test"
SSHAESNI_TEST="sshaesni-test"
SSHAES_PERF="sshaes-perf"
SSHAESNI_PERF="sshaesni-perf"
CPUID_AES="cpuid"

if [ "$1" = "-p" ];
then
    echo "Perfomance mode enabled"
    SEEDS="5"
else
    SEEDS="50"
fi

# Command for running SSHAESNI test
RUNAESNI_TEST=./$SSHAESNI_TEST" "$SEEDS
RUNAESNI_PERF=./$SSHAESNI_PERF

####

echo -n "Checking for binaries... " 
if [ -f $SSHAES_TEST -a -f $SSHAESNI_TEST -a -f $SSHAES_PERF -a -f $SSHAES_PERF ];
then
    echo "found"
else
    echo "not found. Running make all"
    make all
fi

# Build CPUID checker if needed
if [ ! -f $CPUID_AES ];
then
    make cpuid
fi

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
    ./$SSHAES_TEST $SEEDS
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

echo "Performance data generating ..."
if [ -f "perf-original.txt" ];
then
    echo "Original perf data is found, it will be reused"
else
    echo "No original perf file. It will be generated..."
    ./$SSHAES_PERF
    mv perf-output.txt perf-original.txt
fi
echo "Generating aes-ni perf data..."
$RUNAESNI_PERF
