# audiowaveform RPM Packaging

This folder contains scripts and Dockerfiles to create binary RPM packages for Red Hat based systems, including CentOS and Amazon Linux.

## Prepare

Edit the files `build_rpm_centos7.sh`, `build_rpm_centos8.sh`, `build_rpm_almalinux9.sh`, and `build_rpm_amazon_linux_2.sh` to set the versions you want to build:

* Set `AUDIOWAVEFORM_VERSION` to the git branch or tag name (e.g., `master`, `1.5.1`, etc)
* Set `AUDIOWAVEFORM_PACKAGE_VERSION` to the version number for the RPM package

## Build

Use these commands to generate RPM binary packages:

```bash
./build_rpm_centos7.sh
./build_rpm_centos8.sh
./build_rpm_almalinux9.sh
./build_rpm_amazon_linux_2.sh
```

## Output

This will produce RPM files such as:

* audiowaveform-1.5.1-1.el7.x86_64.rpm (CentOS 7)
* audiowaveform-1.5.1-1.el8.x86_64.rpm (CentOS 8)
* audiowaveform-1.5.1-1.el9.x86_64.rpm (AlmaLinux 9)
* audiowaveform-1.5.1-1.amzn2.x86_64.rpm (Amazon Linux 2)

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

For AlmaLinux 9

```bash
yum install -y epel-release
yum localinstall audiowaveform-1.5.1-1.el9.x86_64.rpm
```

For Amazon Linux

```bash
amazon-linux-extras install epel
yum localinstall audiowaveform-1.5.1-1.amzn2.x86_64.rpm
```
