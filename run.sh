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

if [ `uname -m` = x86_64 ];
then
    SDE_BIN=sde64
else
    SDE_BIN=sde
fi

make aescpuid

echo -n "[check] AES-NI support... "
if ./aescpuid -q ;
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
        $SDE_BIN -- ./aescpuid;
    fi
fi

make perf-geomean-aes.txt SDE=$SDE && echo -n "Speedup in geomean is: " && cat perf-geomean-aes.txt
