# audiowaveform Packaging

This folder contains scripts and Dockerfiles to create binary RPM packages for Red Hat based systems, including CentOS, AlmaLinux, and Amazon Linux, DEB packages for Debian.

## Scripts

The following scripts are available:

* `build_deb_debian.sh` - Debian (architectures: `amd64` or `arm64`)
* `build_rpm_centos7.sh` - CentOS 7 (architectures: `x86_64` or `aarch64`)
* `build_rpm_centos8.sh` - CentOS 8 (architectures: `x86_64` or `aarch64`)
* `build_rpm_almalinux9.sh` AlmaLinux 9 (architectures: `x86_64` or `aarch64`)
* `build_rpm_amazon_linux_2.sh` - Amazon Linux 2 (architectures: `x86_64` or `aarch64`)

## Options

Use the following command line options:

* `-v` Sets the audiowaveform package version, e.g., 1.8.0

* `-c` Sets the git commit or tag used to build the package

* `-a` Sets the platform architecture to build, e.g., `x86_64`, `amd64`, `aarch64`, `arm64`, etc. Note that these values vary by Linux distribution

* `-d` Sets the debian release number, for the `build_deb_debian.sh` script, e.g., `10`, `11`, `12`, etc.

## Output

These scripts will produce packages such as:

* audiowaveform-1.5.1-1-12_amd64.deb (Debian 12)
* audiowaveform-1.5.1-1.el7.x86_64.rpm (CentOS 7)
* audiowaveform-1.5.1-1.el8.x86_64.rpm (CentOS 8)
* audiowaveform-1.5.1-1.el9.x86_64.rpm (AlmaLinux 9)
* audiowaveform-1.5.1-1.amzn2.x86_64.rpm (Amazon Linux 2)

These can be uploaded to the [Releases](https://github.com/bbc/audiowaveform/releases) page on GitHub.

## Install

Use these commands to install the packages, together with all required dependencies.

For CentOS 7:

```bash
sudo yum install -y epel-release
sudo yum localinstall audiowaveform-1.5.1-1.el7.x86_64.rpm
```

For CentOS 8:

```bash
sudo yum install -y epel-release
sudo yum localinstall audiowaveform-1.5.1-1.el8.x86_64.rpm
```

For AlmaLinux 9:

```bash
sudo yum install -y epel-release
sudo yum localinstall audiowaveform-1.5.1-1.el9.x86_64.rpm
```

For Amazon Linux:

```bash
sudo amazon-linux-extras install epel
sudo yum localinstall audiowaveform-1.5.1-1.amzn2.x86_64.rpm
```

For Debian 12:

```bash
sudo apt-get update
sudo dpkg -i audiowaveform-1.5.1-1-12.amd64.deb
sudo apt-get -f install -y
```
