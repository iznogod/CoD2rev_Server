#!/bin/bash

cc="g++"
options="-I. -m32 -fPIC -Wall -std=gnu++11"

cc_zlib="gcc"
options_zlib="-I. -m32 -fPIC"

constants=""
pthread_link="-lpthread"

mkdir -p bin
mkdir -p objects
mkdir -p objects/zlib

echo "Compiling /src..."
$cc $options $constants -c src/dvar.cpp -o objects/dvar.o
$cc $options $constants -c src/sv_client.cpp -o objects/sv_client.o
$cc $options $constants -c src/sv_main.cpp -o objects/sv_main.o
$cc $options $constants -c src/shared.cpp -o objects/shared.o
$cc $options $constants -c src/com_memory.cpp -o objects/com_memory.o
$cc $options $constants -c src/math.cpp -o objects/math.o
$cc $options $constants -c src/msg.cpp -o objects/msg.o
$cc $options $constants -c src/netchan.cpp -o objects/netchan.o
$cc $options $constants -c src/sys_thread.cpp -o objects/sys_thread.o
$cc $options $constants -c src/cm_load.cpp -o objects/cm_load.o
$cc $options $constants -c src/filesystem.cpp -o objects/filesystem.o
$cc $options $constants -c src/sys_main.cpp -o objects/sys_main.o
$cc $options $constants -c src/cmd.cpp -o objects/cmd.o
$cc $options $constants -c src/common.cpp -o objects/common.o
$cc $options $constants -c src/sv_game.cpp -o objects/sv_game.o
$cc $options $constants -c src/sys_net.cpp -o objects/sys_net.o

echo "Compiling /src/huffman..."
$cc $options $constants -c src/huffman/huffman.cpp -o objects/huffman.o

echo "Compiling /src/zlib..."
$cc_zlib $options_zlib $constants -c src/zlib/adler32.c -o objects/zlib/adler32.o
$cc_zlib $options_zlib $constants -c src/zlib/crc32.c -o objects/zlib/crc32.o
$cc_zlib $options_zlib $constants -c src/zlib/deflate.c -o objects/zlib/deflate.o
$cc_zlib $options_zlib $constants -c src/zlib/inffast.c -o objects/zlib/inffast.o
$cc_zlib $options_zlib $constants -c src/zlib/inflate.c -o objects/zlib/inflate.o
$cc_zlib $options_zlib $constants -c src/zlib/inftrees.c -o objects/zlib/inftrees.o
$cc_zlib $options_zlib $constants -c src/zlib/unzip.c -o objects/zlib/unzip.o
$cc_zlib $options_zlib $constants -c src/zlib/trees.c -o objects/zlib/trees.o
$cc_zlib $options_zlib $constants -c src/zlib/utils.c -o objects/zlib/utils.o

echo "Linking objects..."
objects="$(ls objects/*.o)"
objects_zlib="$(ls objects/zlib/*.o)"
$cc -m32 -static -L/lib32 -o bin/cod2rev_lnxded -ldl $objects $pthread_link $objects_zlib

echo "Cleaning up..."
rm objects/zlib -r
rm objects -r
echo "Done!"