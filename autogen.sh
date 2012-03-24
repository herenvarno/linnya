#!/bin/sh -e
echo "aclocal..."
aclocal
echo "autoconf..."
autoconf
echo "autoheader..."
autoheader
echo "automake..."
automake --add-missing --copy
echo
echo "Now run: ./configure --prefix="
echo
