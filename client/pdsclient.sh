#!/bin/sh
JAVA=_JAVA_
if test $# -eq 0
then
	echo "Usage: $0 <pds host> <pds port>"
    exit 1
fi

CURDIR=`pwd`
HOMEDIR=`dirname $CURDIR`
LIBDIR=_LIBDIR_
CLASSPATH=$CLASSPATH:_LIBDIR_/pdsclient.jar
LD_LIBRARY_PATH=$LIBDIR:$LD_LIBRARY_PATH
export CLASSPATH
export LD_LIBRARY_PATH
$JAVA -classpath $CLASSPATH client.PDSClient $1 $2 $3 $4 $5 $6
