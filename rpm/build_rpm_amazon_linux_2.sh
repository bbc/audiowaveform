#!/bin/sh -x
#
# Create compile_audiowaveform Docker image, copy results to this directory
# and remove the image afterwards

set -e

AUDIOWAVEFORM_VERSION=1.8.0
AUDIOWAVEFORM_PACKAGE_VERSION=1.8.0
IMAGE=audiowaveform_rpm
docker build -t $IMAGE -f Dockerfile-amazon-linux-2 --build-arg AUDIOWAVEFORM_VERSION=${AUDIOWAVEFORM_VERSION} --build-arg AUDIOWAVEFORM_PACKAGE_VERSION=${AUDIOWAVEFORM_PACKAGE_VERSION} .
CONTAINER_ID=`docker create $IMAGE`
docker cp $CONTAINER_ID:/usr/local/src/audiowaveform-${AUDIOWAVEFORM_VERSION}/build/audiowaveform-${AUDIOWAVEFORM_PACKAGE_VERSION}-1.amzn2.x86_64.rpm .
docker rm -v $CONTAINER_ID
docker rmi $IMAGE
