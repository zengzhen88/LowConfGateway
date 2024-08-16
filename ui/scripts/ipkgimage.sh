#!/bin/sh
if [ $# -lt 3 ];then
    echo "Usage:`basename $0` [-i] pkgname version filelist"
    echo "Options:"
    echo " -i Include pre/postionstall configuration shell"
    exit 1
fi 

tmpdir="/var/tmp/IPKMAKE_$"
mkdir -p $tmpdir
conf=0
if [ $1 == "-i"];then
    conf=1
    shift
fi
name=$1
version=$2
shift
shift

while [ $# -gt 0 ]
do
    if [ -e $1 ];then
        #mkdir -p $tmpdir `dirname $1` &nb sp; cp -r $1 $tmpdir
    fi
    shift
done
curdir=`pwd -L`
cd $tmpdir
tar zcf /var/tmp/IPKDATA$.tar.gz .
rm -rf *
echo "Package: $name" > control
echo "Source: " >> control
echo "priority: " >> control
echo "Section: " >> control
echo "Maintainer" >> control
echo "Architecture: x86" >> control
echo "Version: $version">> control
echo "Depends: " >> control
echo "Description:n-mesh" >> control
echo "size: `du -h ../IPKDATA$.tar.gz|awk '{print $1}'`" >> control
leafpad control

