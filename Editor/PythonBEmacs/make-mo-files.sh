#!/bin/sh
set -e
LOCALEDIR=${1:-missing arg 1 output locale dir}

for PO_LANG in \
    en
do
    MO_OUTPUT_DIR=${LOCALEDIR}/${PO_LANG}/LC_MESSAGES
    mkdir -p ${MO_OUTPUT_DIR}
    if [ ${PO_LANG} = "en" ]
    then
        PO_FILE=I18N/pysvn_workbench_en.current.po
    else
        PO_FILE=I18N/pysvn_workbench_${PO_LANG}.po
    fi
    msgfmt \
        ${PO_FILE} \
        --output-file=${MO_OUTPUT_DIR}/pysvn_workbench.mo

    echo "Info: ${MO_OUTPUT_DIR}/pysvn_workbench.mo" 
done
