#! /bin/bash -e
astyle --style=allman -s4 --recursive "$@" ./*.c ./*.h
