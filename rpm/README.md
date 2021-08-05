# audiowaveform Packaging for CentOS

This folder contains scripts and Dockerfiles to create CentOS binary RPM packages.

## Prepare

Edit the files `build_rpm_centos7.sh` and `build_rpm_centos8.sh` to set the versions you want to build:

* Set `AUDIOWAVEFORM_VERSION` to the git branch or tag name (e.g., `master`, `1.5.1`, etc)
* Set `AUDIOWAVEFORM_PACKAGE_VERSION` to the version number for the RPM package

## Build

Run these commands to generate RPM binary packages for CentOS 7 and 8:

```bash
./build_rpm_centos7.sh
./build_rpm_centos8.sh
```

## Output

This will produce RPM files such as:

* audiowaveform-1.5.1-1.el7.x86_64.rpm (CentOS 7)
* audiowaveform-1.5.1-1.el8.x86_64.rpm (CentOS 8)

These files can be uploaded to the [Releases](https://github.com/bbc/audiowaveform/releases) page on GitHub.

## Install

Use these commands to install the RPM package, together with all required dependencies.

For CentOS 7:

```bash
yum install -y epel-release
yum localinstall audiowaveform-1.5.1-1.el7.x86_64.rpm
```

For CentOS 8:

```bash
yum install -y epel-release
yum localinstall audiowaveform-1.5.1-1.el8.x86_64.rpm
```
