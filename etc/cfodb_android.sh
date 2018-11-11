#!/usr/bin/env bash
TOOLCHAIN=/home/byter/work/android-toolchain/arm64_24
QTANDROID=/home/byter/Qt/5.12.0/android_arm64_v8a

export PATH=$PATH:${TOOLCHAIN}/bin
export target_host=aarch64-linux-android
export AR=$target_host-ar
export AS=$target_host-clang
export CC=$target_host-clang
export CXX=$target_host-clang++
export LD=$target_host-ld
export STRIP=$target_host-strip

# Tell configure what flags Android requires.
export CPPFLAGS="-fPIE -fPIC -Os"
export LDFLAGS="-pie"

if [ $1 == 'ODB' ]
then
 ./bootstrap
 ./configure --disable-shared --host=$target_host --prefix=${TOOLCHAIN}
  make install

elif [ $1 == 'ODBSQL' ]
then
 ./bootstrap
 ./configure --disable-shared --host=$target_host --prefix=${TOOLCHAIN} --with-libodb=/home/byter/work/odb/2.5/libodb-2.5.0-b.3
  make install

elif [ $1 == 'ODBQT' ]
then
 export CPPFLAGS="$CPPFLAGS -I${QTANDROID}/include"
 export LDFLAGS="$LDFLAGS -L${QTANDROID}/lib"

 ./bootstrap
 ./configure --disable-shared --host=$target_host --prefix=${TOOLCHAIN} --with-libodb=/home/byter/work/odb/2.5/libodb-2.5.0-b.3
  make install

elif [ $1 == 'SQL' ]
then
  export CFLAGS="-fPIE -fPIC -Os -DSQLITE_ENABLE_UNLOCK_NOTIFY=1"
  ./configure --host=aarch64-linux-android --prefix=${TOOLCHAIN}
  make install
else
 echo "what you want??"
fi