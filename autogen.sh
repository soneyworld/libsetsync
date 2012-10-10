#!/bin/bash

#Copy Changelog and copyright file to debian folders
cp ChangeLog debian/changelog
cp COPYING debian/copyright
# create version file
. mkversion.sh $
# create version file
. mkversion.sh $@@
if [ ! -d "m4" ]; then
	mkdir "m4"
fi
aclocal -I m4
automake --add-missing
autoreconf --force --install
