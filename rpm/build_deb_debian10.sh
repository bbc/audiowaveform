#!/bin/sh -x
#
# Create compile_audiowaveform Docker image, copy results to this directory
# and remove the image afterwards

set -e

AUDIOWAVEFORM_VERSION=6bec021446bcc8f9da981158130fb200d9fc040a
AUDIOWAVEFORM_PACKAGE_VERSION=1.8.1
IMAGE=audiowaveform_deb
docker build --no-cache -t $IMAGE -f Dockerfile-debian10 --build-arg AUDIOWAVEFORM_VERSION=${AUDIOWAVEFORM_VERSION} --build-arg AUDIOWAVEFORM_PACKAGE_VERSION=${AUDIOWAVEFORM_PACKAGE_VERSION} .
CONTAINER_ID=`docker create $IMAGE`
docker cp $CONTAINER_ID:/usr/local/src/audiowaveform-${AUDIOWAVEFORM_VERSION}/build/audiowaveform_${AUDIOWAVEFORM_PACKAGE_VERSION}-1-10_amd64.deb .
docker rm -v $CONTAINER_ID
docker rmi $IMAGE
