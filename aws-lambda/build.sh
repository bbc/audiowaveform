#!/bin/sh

set -ex

# Build libid3tag
wget https://netix.dl.sourceforge.net/project/mad/libid3tag/0.15.1b/libid3tag-0.15.1b.tar.gz
tar xzf libid3tag-0.15.1b.tar.gz
cd libid3tag-0.15.1b
sed -i 's/ -fforce-mem//' configure
./configure --disable-shared --libdir=/lib64
make install
cd /

# Build libmad
wget https://netix.dl.sourceforge.net/project/mad/libmad/0.15.1b/libmad-0.15.1b.tar.gz
tar xzf libmad-0.15.1b.tar.gz
cd libmad-0.15.1b
sed -i 's/ -fforce-mem//' configure
./configure --disable-shared --libdir=/lib64
make install
cd /

# Build libsndfile (Amazon repo only has earlier 1.0.25 release)
# Requires autogen, automake, and libtool packages
wget https://github.com/erikd/libsndfile/archive/1.0.28.tar.gz
tar xzf 1.0.28.tar.gz
cd libsndfile-1.0.28
sed -i 's/flac >= 1.3.1/flac >= 1.3.0/' configure.ac
./autogen.sh
./configure --disable-shared --libdir=/lib64
make install
cd /

# Build libFLAC
wget https://ftp.osuosl.org/pub/xiph/releases/flac/flac-1.3.0.tar.xz
tar xf flac-1.3.0.tar.xz
cd flac-1.3.0
./configure --disable-shared --libdir=/lib64
make install
cd /

# Build libogg
wget http://downloads.xiph.org/releases/ogg/libogg-1.3.4.tar.gz
tar xf libogg-1.3.4.tar.gz
cd libogg-1.3.4
./configure --disable-shared --libdir=/lib64
make install
cd /

# Build libvorbis
wget http://downloads.xiph.org/releases/vorbis/libvorbis-1.3.6.tar.gz
tar xf libvorbis-1.3.6.tar.gz
cd libvorbis-1.3.6
./configure --disable-shared --libdir=/lib64
make install
cd /

# Build libgd
git clone https://github.com/libgd/libgd.git
cd libgd
mkdir build
cd $_
cmake3 -DBUILD_STATIC_LIBS=1 -DENABLE_PNG=1 ..
make
mv Bin/libgd.a /lib64
cd /

# Build boost
wget https://sourceforge.net/projects/boost/files/boost/1.69.0/boost_1_69_0.tar.gz
tar xf boost_1_69_0.tar.gz
cd boost_1_69_0
./bootstrap.sh --libdir=/lib64 --includedir=/usr/include
./b2 link=static install
cd /

# Build audiowaveform
wget https://github.com/bbc/audiowaveform/archive/1.4.0.tar.gz
tar xzf 1.4.0.tar.gz
mv audiowaveform-1.4.0 audiowaveform
cd $_
mkdir build
cd $_
cmake3 -D ENABLE_TESTS=0 -D BUILD_STATIC=1 ..
make
strip audiowaveform
