#!/bin/bash
# generate.sh
#
# Generator of PuTTY makefiles with AES-NI
#
# @author kryukov@frtk.ru
# @version 1.0
#
# For Putty AES NI project
# http://putty-aes-ni.googlecode.com/

cp ./Putty/Recipe ./Putty/Recipe-backup
perl -p -i -e 's/sshaes/sshaesni/g' ./Putty/Recipe
cd ./Putty/
perl ./mkfiles.pl
mv ./Recipe-backup ./Recipe
cd ..
