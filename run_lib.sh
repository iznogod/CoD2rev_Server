#!/bin/bash

export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:bin"
export LD_PRELOAD="bin/libcod2rev.so"

BASEPATH="bin"
HOMEPATH="bin/.callofduty2"
PARAMS="+set fs_cdpath $BASEPATH +set fs_homepath $HOMEPATH +set fs_game test +set dedicated 2 +set net_port 28962 +set developer 1 +set developer_script 1 +set scr_testclients 1 +set scr_forcerespawn 0 +set g_gametype tdm +devmap mp_toujane"

"bin/cod2_lnxded" "$PARAMS"
