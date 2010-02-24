#!/bin/sh
BIN_DIR=${1:? Bin dir}
${PYTHON} make_be_images.py

cp be_*.py ${BIN_DIR}

echo "#!$( which ${PYTHON} )" >${BIN_DIR}/bemacs
cat ${BIN_DIR}/be_main.py >>${BIN_DIR}/bemacs
rm ${BIN_DIR}/be_main.py
chmod +x ${BIN_DIR}/bemacs
