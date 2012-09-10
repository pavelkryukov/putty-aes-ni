cp ./Putty/Recipe ./Putty/Recipe-backup
perl -p -i -e 's/sshaes/sshaesni/g' ./Putty/Recipe
cd ./Putty/
perl ./mkfiles.pl
mv ./Recipe-backup ./Recipe
cd ..
