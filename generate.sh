#!/bin/bash
# generate.sh
#
# Generator of PuTTY makefiles with AES-NI
#
# @author kryukov@frtk.ru
# @version 2.3
#
# For Putty AES NI project
# http://putty-aes-ni.googlecode.com/

cd ./Putty/
perl -p -i -e 's/-Wall/-Wall -maes -msse4/g' ./mkfiles.pl
perl ./mkfiles.pl
perl -p -i -e 's/-Wall -maes -msse4/-Wall/g' ./mkfiles.pl
cd ..
