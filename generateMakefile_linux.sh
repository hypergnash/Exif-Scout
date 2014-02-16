#!/bin/sh

#check if qmake / qt is available 
dpkg -s "qt5-default" >/dev/null 2>&1 && {
        echo "qt5-default is installed."
    } || {
	dpkg -s "qt4-default" >/dev/null 2>&1 && {
		echo "qt4-default is installed."
   		} || {
       		echo "qt5/4-default is not installed. Lets install it!"
		sudo apt-get install qt5-default
    }
}

#check if exiv2 is available
dpkg -s "exiv2" >/dev/null 2>&1 && {
        echo "exiv2 is installed."
    } || {
	echo "exiv2 is not installed. Lets install it!"
	sudo apt-get install exiv2
}

echo "generating Makefile"
/usr/lib/x86_64-linux-gnu/qt5/bin/qmake -o Makefile Exif-Scout.pro
echo "generating Makefile is done. Use 'make' to build"
