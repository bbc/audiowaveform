#!/bin/sh -x
#
# Create compile_audiowaveform Docker image, copy results to this directory
# and remove the image afterwards

set -e

AUDIOWAVEFORM_VERSION=6bec021446bcc8f9da981158130fb200d9fc040a
AUDIOWAVEFORM_PACKAGE_VERSION=1.8.1
IMAGE=audiowaveform_rpm
CENTOS_RELEASE=8
ARCH=aarch64

docker build \
    --platform linux/${ARCH} \
    --tag ${IMAGE} \
    --file Dockerfile-centos${CENTOS_RELEASE} \
    --build-arg AUDIOWAVEFORM_VERSION=${AUDIOWAVEFORM_VERSION} \
    --build-arg AUDIOWAVEFORM_PACKAGE_VERSION=${AUDIOWAVEFORM_PACKAGE_VERSION} \
    --build-arg CENTOS_RELEASE=${CENTOS_RELEASE} \
    --build-arg ARCH=${ARCH} .
CONTAINER_ID=`docker create $IMAGE`
docker cp $CONTAINER_ID:/root/audiowaveform-${AUDIOWAVEFORM_VERSION}/build/audiowaveform-${AUDIOWAVEFORM_PACKAGE_VERSION}-1.el${CENTOS_RELEASE}.${ARCH}.rpm .
docker rm -v $CONTAINER_ID
docker rmi $IMAGE
