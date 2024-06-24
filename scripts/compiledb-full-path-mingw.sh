#!/bin/sh
[ `uname -o` != "Msys" ] && exit

COMPILEDB_FILE="$1"

fixpath () {
    NEW="C:/msys64`which $1`.exe"
    sed "s#\"$1\",#\"$NEW\",#g" -i "$COMPILEDB_FILE"
}

fixpath arm-none-eabi-gcc
fixpath arm-none-eabi-objcopy
sed 's#"/\(.\)/#"\U\1:/#g' -i "$COMPILEDB_FILE"