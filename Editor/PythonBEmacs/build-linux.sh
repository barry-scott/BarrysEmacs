#!/bin/sh
ROOT_DIR=${1:? Root dir}
BIN_DIR=${1:? Bin dir}
LIB_DIR=${2:? Lib dir}

${PYTHON} make_be_images.py

cp be_*.py ${ROOT_DIR}${LIB_DIR}

cat <<EOF >${ROOT_DIR}${BIN_DIR}/bemacs
#!$( which ${PYTHON} )
import sys
sys.path.insert( 0, "${LIB_DIR}" )
import be_main
sys.exit( be_main.main( sys.argv ) )
EOF
chmod +x ${BIN_DIR}/bemacs
