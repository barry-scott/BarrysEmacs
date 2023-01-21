#!/bin/bash
X=$( ps -u $(id -u) -o pid,comm|grep be_main.app/Contents/MacOS/be_main )
PID=${X% *}
EXE=${X#* }
rm -f .gdbinit
gdb --pid=${PID} ${EXE}
