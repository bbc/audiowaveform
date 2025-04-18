#!/bin/bash
#
# This script creates a source package for several Ubuntu versions,
# each with its own debian/changelog file.
#
#-------------------------------------------------------------------------------

set -e

PROJECT=audiowaveform
REPO=git@github.com:bbc/${PROJECT}.git
TAG=1.10.2
SOURCE_DIR=${PROJECT}_${TAG}
TARBALL=${SOURCE_DIR}.orig.tar.gz
PACKAGES_DIR=packages
PPA=ppa:chris-needham/ppa

# 12.04 Precise (ignore because of libgd2-xpm-dev, later Ubuntu releases have libgd-dev)
# 14.04 Trusty
# 16.04 Xenial
# 17.10 Artful (obsolete and will not accept new uploads)
# 18.04 Bionic
# 18.10 Cosmic (obsolete and will not accept new uploads)
# 19.04 Disco (obsolete and will not accept new uploads)
# 19.10 Eoan (obsolete and will not accept new uploads)
# 20.04 Focal
# 20.10 Groovy (obsolete and will not accept new uploads)
# 21.04 Hirsute (obsolete and will not accept new uploads)
# 21.10 Impish (obsolete and will not accept new uploads)
# 22.04 Jammy
# 22.10 Kinetic (obsolete and will not accept new uploads)
# 23.04 Lunar (obsolete and will not accept new uploads)
# 24.04 Noble

declare -a ubuntu_releases=("trusty" "xenial" "bionic" "focal" "jammy" "noble")

cleanup() {
    rm -rf ${SOURCE_DIR}/.git
    rm -rf ${SOURCE_DIR}/.github
    rm -rf ${SOURCE_DIR}/.gitignore
    rm -rf ${SOURCE_DIR}/doc/example.png
    rm -rf ${SOURCE_DIR}/doc/DataFormat.md
    rm -rf ${SOURCE_DIR}/Doxyfile
    rm -rf ${SOURCE_DIR}/rpm
    rm -rf ${SOURCE_DIR}/test
    rm -rf ${SOURCE_DIR}/ubuntu
}

fixup() {
    # TODO: move the changelog.template file elsewhere
    sed -E "s/RELEASE/$1/g" changelog.template > ${SOURCE_DIR}/debian/changelog
}

sourcepackage() {
    tar --create --verbose --gzip \
        --exclude=.git \
        --exclude=.gitignore \
        --exclude=.github \
        --exclude=debian \
        --exclude=doc/example.png \
        --exclude=doc/DataFormat.md \
        --exclude=Doxyfile \
        --exclude=rpm \
        --exclude=test \
        --exclude=ubuntu \
        --file ${TARBALL} ${SOURCE_DIR}
}

deb() {
    pushd ${SOURCE_DIR}
    # -S:  Build source package
    # -sa: This option is passed by debuild to dpkg-buildpackage, which passes it
    #      through to dpkg-genchanges. It "forces the inclusion of the original
    #      source".


    # audiowaveform_1.2.2-1trusty1.debian.tar.xz
    # audiowaveform_1.2.2-1trusty1.dsc
    # audiowaveform_1.2.2-1trusty1_source.build
    # audiowaveform_1.2.2-1trusty1_source.changes
    # -- Debian upload control file, produced by dpkg-genchanges

    debuild -S -sa
    popd
}

movefiles() {
    mkdir -p packages
    cp ${TARBALL} packages
    mv *.dsc packages
    mv *.build packages
    mv *.buildinfo packages
    mv *.changes packages
    mv *.tar.xz packages
}

debs() {
    if [ -n "$1" ]; then
        cp packages/${TARBALL} .
        fixup "$1"
        deb
        movefiles "$1"
    else
        for ubuntu_release in "${ubuntu_releases[@]}"
        do
            fixup "$ubuntu_release"
            deb
            movefiles "$ubuntu_release"
        done

        rm ${TARBALL}
    fi
}

publish() {
    pushd ${PACKAGES_DIR}

    if [ -n "$1" ]; then
        dput ${PPA} ${PROJECT}_${TAG}-1${1}1_source.changes
    else
        for ubuntu_release in "${ubuntu_releases[@]}"
        do
            dput ${PPA} ${PROJECT}_${TAG}-1${ubuntu_release}1_source.changes
        done
    fi

    popd
}

case "$1" in
    checkout)
        git clone --branch ${TAG} --depth 1 ${REPO} ${SOURCE_DIR}
        cp ${SOURCE_DIR}/debian/changelog changelog.template
        cleanup
        ;;

    sourcepackage)
        sourcepackage
        ;;

    foo)
        foo "$2"
        ;;

    debs)
        debs "$2"
        ;;

    publish)
        publish "$2"
        ;;

    clean)
        rm ${TARBALL}
        rm -rf packages
        ;;

    *)
        cat <<END
Usage: package.sh checkout|sourcepackage|debs|publish|clean

END
        exit 1
        ;;
esac
