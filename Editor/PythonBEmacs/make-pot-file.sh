#!/bin/bash

ls *.py | grep -v -e ^wb_diff_images.py -e ^wb_images.py >wb_files.tmp

xgettext \
    --files-from=wb_files.tmp \
    --default-domain=pysvn_workbench \
    --output=I18N/pysvn_workbench.current.pot \
    --msgid-bugs-address=barryscott@tigris.org \
    --copyright-holder="Barry Scott" \
    --keyword=U_ \
    --keyword=T_ \
    --keyword=S_:1,2 \
    --add-comments \
    --no-wrap \
    --add-comments=Translat \
    --language=Python
