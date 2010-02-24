#!/bin/sh
BIN_DIR=${1:? Bin dir}
${PYTHON} make-images.py

cp be_*.py ${BIN_DIR}

echo "#!${PYTHON}" >${BIN_DIR}/bemacs
cat ${BIN_DIR}/be_main.py >>${BIN_DIR}/bemacs
rm ${BIN_DIR}/be_main.py
