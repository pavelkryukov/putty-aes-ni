#!/bin/bash
# run.sh
#
# Run of unit tests
#
# @author kryukov@frtk.ru
# @author Maxim Kuznetsov <maks.kuznetsov@gmail.com>
# @version 1.0
#
# For Putty AES NI project
# http://putty-aes-ni.googlecode.com/

SSHAES="sshaes"
SSHAESNI="sshaesni"
CPUID_AES="cpuid"

# Command for running SSHAESNI test
RUNAESNI=./$SSHAESNI

####

echo -n "Checking for $SSHAES and $SSHAESNI... " 
if [ -f $SSHAES -a -f $SSHAESNI ];
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
            RUNAESNI="sde -- "$RUNAESNI
        fi
fi

####

if [ -f "original.txt" ];
then
    echo "Original trace is found, it will be reused"
else
    echo "No original trace file. It will be generated..."
    ./$SSHAES
    mv output.txt original.txt
fi

echo "Generating aes-ni trace..."
$RUNAESNI

####

echo "MD5 check..."
original=$(md5sum original.txt)
changed=$(md5sum output.txt)

if [ "$original" = "$changed" ];
then 
    echo "Tests passed!"
else
    vim -d original.txt output.txt
fi
