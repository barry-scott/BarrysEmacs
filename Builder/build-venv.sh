#!/bin/bash

if ! which colour-print >/dev/null
then
    function colour-print {
        echo "$@"
    }
fi

case "$(uname)" in
Darwin)
    requirements_file=macos-requirements.txt
    ;;

*)
    colour-print "<>error Error:<> not support on $(uanme)"
    exit 1
    ;;
esac

colour-print "<>info Info:<> Clean up"
rm -rf venv.tmp

colour-print "<>info Info:<> Create venv for $PYTHON"
$PYTHON -m venv venv.tmp

colour-print "<>info Info:<> Install requirements"

venv.tmp/bin/python -m pip install --upgrade pip setuptools
venv.tmp/bin/python -m pip install wheel
venv.tmp/bin/python -m pip install -r ${requirements_file}
venv.tmp/bin/python -m pip list
