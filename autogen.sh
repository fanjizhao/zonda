#!/bin/sh

mkdir -p m4
aclocal -I m4
libtoolize --force 
autoconf --force 
autoheader --force 
automake --force --add-missing
