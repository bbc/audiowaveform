#!/bin/bash
#
# Create Docker image, copy results to this directory
# and remove the image afterwards

set -e

source ./cmdline.sh

if [ -z "${DEBIAN_RELEASE}" ]
then
    echo "Missing debian release number (e.g., 10, 11, 12, or 13)"
    exit 1
fi

if [ -z "${ARCH}" ]
then
    echo "Missing architecture (amd64 or arm64)"
    exit 1
fi

IMAGE=audiowaveform_deb

set -x

docker buildx build \
    --progress plain \
    --platform linux/${ARCH} \
    --tag ${IMAGE} \
    --file Dockerfile-debian \
    --build-arg AUDIOWAVEFORM_VERSION=${AUDIOWAVEFORM_VERSION} \
    --build-arg AUDIOWAVEFORM_PACKAGE_VERSION=${AUDIOWAVEFORM_PACKAGE_VERSION} \
    --build-arg DEBIAN_RELEASE=${DEBIAN_RELEASE} \
    --build-arg ARCH=${ARCH} .
CONTAINER_ID=`docker create ${IMAGE}`
docker cp ${CONTAINER_ID}:/root/audiowaveform-${AUDIOWAVEFORM_VERSION}/build/audiowaveform_${AUDIOWAVEFORM_PACKAGE_VERSION}-1-${DEBIAN_RELEASE}_${ARCH}.deb .
docker rm -v ${CONTAINER_ID}
docker rmi ${IMAGE}
