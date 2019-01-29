#!/bin/sh

if [ "$#" = "0" ]; then
	echo Usage : "$0" bmp_fname_without_ext
	exit
fi

b2r "$1".bmp
r2b "$1" -cfg i80.cfg -of "$1"_x
