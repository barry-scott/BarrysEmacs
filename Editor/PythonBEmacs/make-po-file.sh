#!/bin/sh
set -e
WB_LOCALE=${1?locale missing}

if [ -e I18N/pysvn_workbench_${WB_LOCALE}.po ]
then
    echo Info: Update ${WB_LOCALE} from I18N/pysvn_workbench.current.pot
    msginit \
        --input=I18N/pysvn_workbench.current.pot \
        --locale=${WB_LOCALE} \
        --no-wrap \
        --no-translator \
        --output-file=I18N/pysvn_workbench_${WB_LOCALE}.tmp.po

    msgmerge \
        I18N/pysvn_workbench_${WB_LOCALE}.po \
        I18N/pysvn_workbench_${WB_LOCALE}.tmp.po \
        --quiet \
        --no-wrap \
        --output-file=I18N/pysvn_workbench_${WB_LOCALE}.current.po

else
    echo Info: Create ${WB_LOCALE} from I18N/pysvn_workbench.current.pot
    msginit \
        --input=I18N/pysvn_workbench.current.pot \
        --locale=${WB_LOCALE}.UTF-8 \
        --no-wrap \
        --no-translator \
        --output-file=I18N/pysvn_workbench_${WB_LOCALE}.current.po
fi

echo Info: Version brand ${WB_LOCALE} from I18N/pysvn_workbench.current.pot
python <<EOF
import wb_version
import datetime
all_po_lines = file( "I18N/pysvn_workbench_${WB_LOCALE}.current.po", 'r' ).readlines()

for index, line in enumerate( all_po_lines ):
    if line.startswith( '"Project-Id-Version:' ):
        all_po_lines[ index ] = ('"Project-Id-Version: WorkBench %d.%d.%d.%d\\\\n"\\n' % 
            (wb_version.major
            ,wb_version.minor
            ,wb_version.patch
            ,wb_version.build))
    elif line.startswith( '"PO-Revision-Date:' ):
        all_po_lines[ index ] = '"PO-Revision-Date: %s\\\\n"\\n' % (datetime.datetime.now().isoformat(' '),)
    elif line.startswith( '"Content-Type: text/plain; charset=ASCII\\\\n"' ):
        all_po_lines[ index ] = '"Content-Type: text/plain; charset=UTF-8\\\\n"\\n'

file( "I18N/pysvn_workbench_${WB_LOCALE}.current.po", 'w' ).write( ''.join( all_po_lines ) )
EOF
