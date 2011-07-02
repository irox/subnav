#!/bin/sh
aclocal
automake --add-missing --foreign
libtoolize
autoconf
