FROM amazonlinux:latest
RUN yum -y update
RUN yum -y install make cmake cmake3 autogen automake libtool gcc gcc-c++ wget tar \
  gzip zip libcurl-devel zlib-static libpng-static xz git boost-static boost-devel python-devel \
  bzip2-devel which gd-devel

ARG BUILD_DIR=/build
ARG SCRIPTS_DIR=/scripts

ENV BUILD_DIR=$BUILD_DIR
ENV SCRIPTS_DIR=$SCRIPTS_DIR

RUN mkdir $BUILD_DIR && mkdir $SCRIPTS_DIR

WORKDIR /root

CMD $SCRIPTS_DIR/build.sh
