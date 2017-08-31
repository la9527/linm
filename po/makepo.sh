#!/bin/sh

cd ..

FIND_DIR=.
CPWD=$(pwd)

for i in $(find $FIND_DIR -type f | grep '\.cpp$' | grep -v 'svn')
do
    echo "$i"
    xgettext -k_ $i -j -o ./po/ko.po_new
done

for i in $(find $FIND_DIR -type f | grep '\.h$' | grep -v 'svn')
do
    echo "$i"
    xgettext -k_ $i -j -o ./po/ko.po_new
done 

for i in $(find $FIND_DIR -type f | grep '\.cfg$' | grep -v 'svn')
do
    echo "$i"
    xgettext -k_ $i -j -o ./po/ko.po_new
done 

