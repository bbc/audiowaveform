#!/bin/bash
#
# Create Docker image, copy results to this directory
# and remove the image afterwards

set -e

source ./cmdline.sh

IMAGE=audiowaveform_rpm
AMAZON_RELEASE=2

if [ -z "${ARCH}" ]
then
    echo "Missing architecture (x86_64 or aarch64)"
    exit 1
fi

set -x

docker buildx build \
    --progress plain \
    --platform linux/${ARCH} \
    --tag ${IMAGE} \
    --file Dockerfile-amazon-linux-${AMAZON_RELEASE} \
    --build-arg AUDIOWAVEFORM_VERSION=${AUDIOWAVEFORM_VERSION} \
    --build-arg AUDIOWAVEFORM_PACKAGE_VERSION=${AUDIOWAVEFORM_PACKAGE_VERSION} \
    --build-arg AMAZON_RELEASE=${AMAZON_RELEASE} \
    --build-arg ARCH=${ARCH} .
CONTAINER_ID=`docker create ${IMAGE}`
docker cp ${CONTAINER_ID}:/root/audiowaveform-${AUDIOWAVEFORM_VERSION}/build/audiowaveform-${AUDIOWAVEFORM_PACKAGE_VERSION}-1.amzn${AMAZON_RELEASE}.${ARCH}.rpm .
docker rm -v ${CONTAINER_ID}
docker rmi ${IMAGE}
