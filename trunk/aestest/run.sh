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

if [ -f "sshaesni" ];
then
    echo "sshaeni is found"
    if [ -f "sshaes" ]; 
    then
        echo "sshaes is found"
    else
        echo "sshaes is not found. Running make all"
        make all
    fi
else
    echo "sshaesni.out is not found. Running make all"
    make all
fi

if [ -f "original.txt" ];
then
    echo "Original trace is found, it will be reused"
else
    echo "No original trace file. It will be generated..."
    ./sshaes
    mv output.txt original.txt
fi

echo "Generating local trace..."
./sshaesni original.txt

echo "MD5 check..."
original=$(md5 original.txt)
changed=$(md5 output.txt)

if [ "$a" = "$b" ]
then 
    echo "Tests passed!"
else
    vim -d original.txt output.txt
fi
