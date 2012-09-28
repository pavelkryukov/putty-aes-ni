#!/bin/bash
# run.sh
#
# Run of unit tests
#
# @author kryukov@frtk.ru
# @version 1.0
#
# For Putty AES NI project
# http://putty-aes-ni.googlecode.com/

SSHAES="sshaes"
SSHAESNI="sshaesni"
CPUID_AES="cpuid"

# Command for running SSHAESNI test
RUNAESNI=./$SSHAESNI

# 
if [ -f $SSHAES -a -f $SSHAESNI ];
then
    echo "$SSHAES and $SSHAESNI are found"
else
    echo "$SSHAES or $SSHAESNI not found. Running make all"
    make all
fi

# Build CPUID checker if needed
if [ ! -f $CPUID_AES ];
then
    make cpuid
fi

# Check for AES-NI support
if ./cpuid -q ;
then
    echo "Your CPU supports AES-NI"
else
     echo "Your CPU doesn't support AES-NI, using SDE"
     RUNAESNI="sde -- "$RUNAESNI
fi

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

echo "MD5 check..."
original=$(md5sum original.txt)
changed=$(md5sum output.txt)

if [ "$original" = "$changed" ];
then 
    echo "Tests passed!"
else
    vim -d original.txt output.txt
fi
