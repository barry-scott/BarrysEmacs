#!/bin/bash
#export BEMACS_FIFO=.bemacs8devel/.emacs_command
python ${BUILDER_TOP_DIR}/Editor/PyQtBEmacs/be_client.py "$@"
