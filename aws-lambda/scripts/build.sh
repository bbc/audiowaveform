#!/bin/sh

set -ex

# Build libid3tag
cd $BUILD_DIR
wget https://netix.dl.sourceforge.net/project/mad/libid3tag/0.15.1b/libid3tag-0.15.1b.tar.gz
tar xzf libid3tag-0.15.1b.tar.gz
cd libid3tag-0.15.1b
sed -i 's/ -fforce-mem//' configure
./configure --disable-shared --prefix=/usr --libdir=/usr/lib64
make install

# Build libmad
cd $BUILD_DIR
wget https://netix.dl.sourceforge.net/project/mad/libmad/0.15.1b/libmad-0.15.1b.tar.gz
tar xzf libmad-0.15.1b.tar.gz
cd libmad-0.15.1b
sed -i 's/ -fforce-mem//' configure
./configure --disable-shared --prefix=/usr --libdir=/usr/lib64
make install

# Build libogg
cd $BUILD_DIR
wget http://downloads.xiph.org/releases/ogg/libogg-1.3.4.tar.gz
tar xf libogg-1.3.4.tar.gz
cd libogg-1.3.4
./configure --disable-shared --prefix=/usr --libdir=/usr/lib64
make install

# Build libvorbis
cd $BUILD_DIR
wget http://downloads.xiph.org/releases/vorbis/libvorbis-1.3.6.tar.gz
tar xf libvorbis-1.3.6.tar.gz
cd libvorbis-1.3.6
./configure --disable-shared --prefix=/usr --libdir=/usr/lib64
make install

# Build libFLAC
cd $BUILD_DIR
wget https://ftp.osuosl.org/pub/xiph/releases/flac/flac-1.3.3.tar.xz
tar xf flac-1.3.3.tar.xz
cd flac-1.3.3
./configure --disable-shared --prefix=/usr --libdir=/usr/lib64
make install

# Build libgd
cd $BUILD_DIR
wget https://github.com/libgd/libgd/releases/download/gd-2.2.5/libgd-2.2.5.tar.gz
tar xzf libgd-2.2.5.tar.gz
cd libgd-2.2.5
mkdir -p build
cd build
cmake -DBUILD_STATIC_LIBS=1 -DENABLE_PNG=1 ..
make
make install

# Build libsndfile (Amazon repo only has earlier 1.0.25 release)
# Requires autogen, automake, and libtool packages
cd $BUILD_DIR
wget https://github.com/erikd/libsndfile/archive/1.0.28.tar.gz
tar xzf 1.0.28.tar.gz
cd libsndfile-1.0.28
./autogen.sh
./configure --disable-shared --prefix=/usr --libdir=/usr/lib64
make install

# Build audiowaveform
cd $BUILD_DIR
wget https://github.com/bbc/audiowaveform/archive/1.4.1.tar.gz
tar xzf 1.4.1.tar.gz
cd audiowaveform-1.4.1
sed -i 's/ -no-pie//' CMakeLists.txt
mkdir -p build
cd build
cmake -D ENABLE_TESTS=0 -D BUILD_STATIC=1 ..
make
strip audiowaveform
# Check dependencies
ldd audiowaveform
# Test
./audiowaveform -i ../test/data/test_file_stereo.flac -o test.dat -b 8 && cp audiowaveform -t $BUILD_DIR
