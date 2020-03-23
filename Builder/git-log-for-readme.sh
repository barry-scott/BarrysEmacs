#!/bin/bash
last_tag=$( git describe --tags --abbrev=0 )
cd ${BUILDER_TOP_DIR}
git log ${last_tag}..HEAD --name-only >Builder/tmp/all.log
git log ${last_tag}..HEAD --name-only -- MLisp >Builder/tmp/mlisp.log
git log ${last_tag}..HEAD --name-only -- Editor >Builder/tmp/editor.log
git log ${last_tag}..HEAD --name-only -- Describe HTML >Builder/tmp/docs.log

ls -1 ${BUILDER_TOP_DIR}/Builder/tmp/*.log
