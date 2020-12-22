#!sh

cd $1

for i in *.$2 ; do convert $i `echo $i | perl -pe s/$2$/png/i`; done; rm *.$2
cat *.rc | sed s/\.$2/\.png/g > temp ; mv temp *.rc




