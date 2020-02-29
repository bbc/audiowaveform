FROM amazonlinux:1
RUN yum -y update
RUN yum -y install make wget tar gcc gcc-c++ file diffutils
RUN wget https://cmake.org/files/v3.7/cmake-3.7.2.tar.gz && \
  tar -xzvf cmake-3.7.2.tar.gz && cd cmake-3.7.2 && \
  ./bootstrap && make && make install
RUN yum -y install cmake autogen automake libtool \
  gzip zip libcurl-devel zlib-static libpng-static xz git \
  boost-static boost-devel \
  bzip2-devel which gd-devel

ARG BUILD_DIR=/build
ARG SCRIPTS_DIR=/scripts

ENV BUILD_DIR=$BUILD_DIR
ENV SCRIPTS_DIR=$SCRIPTS_DIR

RUN mkdir $BUILD_DIR && mkdir $SCRIPTS_DIR

WORKDIR /root

CMD $SCRIPTS_DIR/build.sh
