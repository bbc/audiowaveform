FROM ubuntu:trusty

MAINTAINER Thomas Parisot <thomas.parisot@bbc.co.uk>

ENV GMOCK_VERSION 1.7.0
ENV BUILD_TYPE Release

RUN apt-get update \
  && apt-get install -y cmake \
			make \
			g++ \
			gcc \
			libmad0-dev \
			libsndfile1-dev \
			libgd2-xpm-dev \
			libboost-filesystem-dev \
			libboost-program-options-dev \
			libboost-regex-dev \
			git-core

WORKDIR /audiowaveform

COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./COPYING ./COPYING
COPY ./README.md ./README.md
COPY ./VERSION ./VERSION
COPY ./debian ./debian
COPY ./doc ./doc
COPY ./cmake ./cmake
COPY ./src ./src
COPY ./test ./test

WORKDIR build

RUN cmake -D CMAKE_BUILD_TYPE=${BUILD_TYPE} -D ENABLE_TESTS=0 .. \
  && make \
  && make install

ENTRYPOINT ["audiowaveform"]


