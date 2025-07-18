#!/usr/bin/bash
#
buildDir=buildDir
rm -rf $buildDir
mkdir $buildDir
QTDIR=/opt/Qt/6.9.1/gcc_64

/opt/Qt/Tools/CMake/bin/cmake -DCMAKE_PREFIX_PATH=$QTDIR -S . -B $buildDir
cd $buildDir
make

