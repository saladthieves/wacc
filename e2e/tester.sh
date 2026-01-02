#! /bin/sh

COMMAND=$1
OUTPUT=$2

eval ${COMMAND}
echo $? > $OUTPUT