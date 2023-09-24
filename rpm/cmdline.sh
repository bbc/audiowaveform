#!/bin/sh

show_help() {
    echo "Usage:"
    echo "  -h --help    Show help"
    echo "  -v --version audiowaveform package version (e.g., 1.8.1)"
    echo "  -c --commit  audiowaveform commit hash or tag name (e.g., 1.8.1)"
    echo "  -d --debian  Debian release number (e.g., 8, 9, or 10)"
    echo "  -a --arch    Architecture (e.g., amd64, x86_64, arm64)"
}

while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 1
            ;;
        -v|--version)
            AUDIOWAVEFORM_PACKAGE_VERSION="$2"
            shift
            shift
            ;;
        -c|--commit)
            AUDIOWAVEFORM_VERSION="$2"
            shift
            shift
            ;;
        -a|--arch)
            ARCH="$2"
            shift
            shift
            ;;
        -d|--debian)
            DEBIAN_RELEASE="$2"
            shift
            shift
            ;;
        -*|--*)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

if [ -z "${AUDIOWAVEFORM_PACKAGE_VERSION}" ]
then
    echo "Missing package version"
    exit 1
fi

if [ -z "${AUDIOWAVEFORM_VERSION}" ]
then
    echo "Missing git tag / commit"
    exit 1
fi

echo "AUDIOWAVEFORM_PACKAGE_VERSION: ${AUDIOWAVEFORM_PACKAGE_VERSION}"
echo "AUDIOWAVEFORM_VERSION: ${AUDIOWAVEFORM_VERSION}"
