#!/bin/ksh
set -x

osname=$( uname )

edit_obj=obj/$osname/
edit_exe=exe/$osname/

rootdir=$PWD
build_opt=${1:-.pure}
export emacs_library=~/bemacs/kit
export emacs_user=$rootdir/test_data-$osname
export emacs_path="emacs_user: emacs_library:"
export envfile=$emacs_user/coverage.emacs_environment.tmp
imagefile=$rootdir/${edit_obj}emacs${build_opt}

rm -rf $emacs_user
mkdir -p $emacs_user

echo "(message \"Testing emacs\")" >$emacs_user/emacsinit.ml
cp coverage.mlp $emacs_user

cd $emacs_user

export pass=${2:-1}
dbg=${3:-0}
if (( pass == 1 ))
then
    echo Pass 1
    rm -f ${edit_obj}emacs_pure.pcv
    rm -f $envfile
    rm -f *.ej*
    rm -f *.tmp
    cat >file.c <<z_z_z
int main()
    {
    return 0;
    }
z_z_z
    export EMACS_DEBUG=0x10000000    # unused debug value
    if (( $dbg == $pass ))
    then
        echo b purify_stop_here >coverage.playback.tmp
        echo "D \"$rootdir\"" >>coverage.playback.tmp
        echo b debug_invoke >>coverage.playback.tmp
        echo r -nodisplay -package=coverage -norestore file.c >>coverage.playback.tmp
        xdb -p coverage.playback.tmp $imagefile
    else
        $imagefile -nodisplay -package=coverage -norestore file.c
        pass=2
    fi
    ll $envfile
fi
if (( pass == 2 ))
then
    echo Pass 2
    dbcreate testdb1 -c
    dbcreate testdb2 -c
    if (( $dbg == $pass ))
    then
        echo b purify_stop_here >coverage.playback.tmp
        echo "D \"$rootdir\"" >>coverage.playback.tmp
        echo b debug_invoke >>coverage.playback.tmp
        echo r -package=coverage -restore=$envfile >>coverage.playback.tmp
        xdb -p coverage.playback.tmp $imagefile
    else
        $imagefile -package=coverage -restore=$envfile
        pass=3
    fi
fi
if (( pass == 3 ))
then
    echo Pass 3
    if (( $dbg == $pass ))
    then
        echo b purify_stop_here >coverage.playback.tmp
        echo "D \"$rootdir\"" >>coverage.playback.tmp
        echo b debug_invoke >>coverage.playback.tmp
        echo r -package=coverage -restore=$envfile >>coverage.playback.tmp
        xdb -p coverage.playback.tmp $imagefile
    else
        $imagefile -package=coverage -restore=$envfile
        pass=4
    fi
fi
more purify-coverage.phase-2-results.tmp
echo Done.
