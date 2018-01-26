%define		src	%{_topdir}/../../
%define		google_test_version	1.8.0

Name:           audiowaveform 
Version:	%{_version}
Release:        1%{?dist}
Summary:        application that generates waveform data from either MP3, WAV, or FLAC format audio files

License:        GNU GPL 3
URL:            https://github.com/bbc/audiowaveform
Source0:        https://github.com/google/googletest/archive/release-%{google_test_version}.tar.gz

BuildRequires:  make cmake gcc-c++ libmad-devel libid3tag-devel libsndfile-devel gd-devel boost-devel 

%description
audiowaveform is a C++ command-line application that generates waveform data from either MP3, WAV, or FLAC format audio files. Waveform data can be used to produce a visual rendering of the audio, similar in appearance to audio editing applications.

%prep
%setup -q -n googletest-release-%{google_test_version}
ln -sf `pwd`/googletest %{src}/
ln -sf `pwd`/googlemock %{src}/

%build
mkdir build
cd build
cmake -D CMAKE_INSTALL_PREFIX=%{buildroot}/usr %{src}
make


%install
cd build
make install
cd %{buildroot}
find -type f | sed 's#^\.##g' > /tmp/%{name}-files.txt 


%files -f /tmp/%{name}-files.txt
%attr(555,-,-) /usr/bin/audiowaveform

%changelog
