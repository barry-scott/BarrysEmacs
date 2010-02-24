#!/bin/sh
ROOT_DIR=${1:? Root dir}
ROOT_BIN_DIR=${2:? Bin dir}
ROOT_LIB_DIR=${3:? Lib dir}

${PYTHON} make_be_images.py

cp be_*.py ${ROOT_LIB_DIR}

LIB_DIR=${ROOT_LIB_DIR#${ROOT_DIR}*}

cat <<EOF >${ROOT_BIN_DIR}/bemacs
#!$( which ${PYTHON} )
import sys
sys.path.insert( 0, "${LIB_DIR}" )
import be_main
sys.exit( be_main.main( sys.argv ) )
EOF
chmod +x ${ROOT_BIN_DIR}/bemacs
