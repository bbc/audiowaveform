# builder image to build the audiowaveform RPM
FROM centos:7 AS audiowaveform-builder

# Use the epel repository (to provide the libmad package)
RUN INSTALL_PKGS="epel-release" && \
    yum install -y --setopt=tsflags=nodocs  $INSTALL_PKGS && rpm -V $INSTALL_PKGS && \
    yum clean all -y

# Install audiowaveform build dependencies
RUN INSTALL_PKGS="redhat-lsb-core rpm-build wget git make cmake gcc-c++ libmad-devel libid3tag-devel libsndfile-devel gd-devel boost-devel libmad-devel" && \
    yum install -y --setopt=tsflags=nodocs  $INSTALL_PKGS && rpm -V $INSTALL_PKGS && \
    yum clean all -y

# Retrieve and build source (see https://github.com/bbc/audiowaveform#building-from-source)
COPY . /usr/local/src/audiowaveform/
RUN cd /usr/local/src/audiowaveform && \
    wget -qO- https://github.com/google/googletest/archive/release-1.11.0.tar.gz | tar xfz - -C . && \
    mv -f googletest-* googletest && \
    mkdir build && \
    cd build && \
    cmake -D ENABLE_TESTS=0 .. && \
    make package && \
    mkdir /target && \
    cp -v *.rpm /target && \
    cd /target && \
    rm -rf /usr/local/src/audiowaveform && \
    rpm -Uvh /target/*.rpm && \
    audiowaveform --version

FROM centos:7

# Use the epel repository (to provide the libmad package)
RUN INSTALL_PKGS="epel-release" && \
    yum install -y --setopt=tsflags=nodocs  $INSTALL_PKGS && rpm -V $INSTALL_PKGS && \
    yum clean all -y

# Install audiowaveform from the RPM in the above audiowaveform-builder image
COPY --from=audiowaveform-builder /target/audiowave*.rpm /tmp/rpms/audiowave.rpm
RUN INSTALL_PKGS="libmad libsndfile libid3tag gd boost" && \
    yum install -y --setopt=tsflags=nodocs $INSTALL_PKGS && rpm -V $INSTALL_PKGS && \
    rpm -ivh --excludedocs /tmp/rpms/*.rpm && \
    rm -rf /tmp/rpms && \
    yum clean all -y

ENTRYPOINT ["/usr/bin/audiowaveform"]
