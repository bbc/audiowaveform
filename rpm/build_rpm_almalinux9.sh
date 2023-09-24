#!/bin/bash -x
#
# Create compile_audiowaveform Docker image, copy results to this directory
# and remove the image afterwards

set -e

source ./cmdline.sh

IMAGE=audiowaveform_rpm
ARCH=x86_64

set -x

docker build \
    --progress plain \
    --tag ${IMAGE} \
    --file Dockerfile-almalinux9 \
    --build-arg AUDIOWAVEFORM_VERSION=${AUDIOWAVEFORM_VERSION} \
    --build-arg AUDIOWAVEFORM_PACKAGE_VERSION=${AUDIOWAVEFORM_PACKAGE_VERSION}
    --build-arg ARCH=${ARCH} .
CONTAINER_ID=`docker create $IMAGE`
docker cp ${CONTAINER_ID}:/usr/local/src/audiowaveform-${AUDIOWAVEFORM_VERSION}/build/audiowaveform-${AUDIOWAVEFORM_PACKAGE_VERSION}-1.el9.${ARCH}.rpm .
docker rm -v ${CONTAINER_ID}
docker rmi ${IMAGE}
