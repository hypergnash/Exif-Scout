Exif-Scout
==========

The Exif-Scout is a C++ code GUI for offline searching through the exif-data of images. You can select various exif-data as search parameters and the Exif-Scout searches through your offline media.

Exif-Scout is a program I have written as a universities class project some years ago. I wanted to save it before it gets lost. Therefore you have the oppotunity to check Exif-Scout out! Open Exif-Scout-GUI.png for GUI screenshot.


--project by 	Tim Luedtke (mail «at» timluedtke dot de)
--thanks to  	S. Brueckner, S. Walz, R. Kratou



WARNING
=======

This project is not jet in a working condition because the makefiles need to be adjusted. As a result of bad makefile generation (by eclipse) the makefiles have the needed directorys hardcoded (e.g. /usr/share/qt4/mkspecs/linux-g++). Therefore it is some work necessary before the project can be compiled for the first time after the years. Sorry for that - but thats over my makefile-knowledge for know... Some help here would be nice.

The Exiv2-Library may be needed on you system (http://www.exiv2.org/download.html).



FILES
=====

Makefile 		- was once used for comiling in linux
Makefile.Debug		- was once used for comiling in Windows
Makefile.Release	- was once used for comiling in Windows

exivdata.cpp		- file used from the exif2-library for reading out the exif-data from files



LICENCE
=======

This project is licensed under the GPLv3. 

All images used in this project are selfmade by the projects author and licensed under the same license.

This project also uses parts of the Exiv2-Library (http://www.exiv2.org/).
 

