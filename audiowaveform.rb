require 'formula'

class Audiowaveform < Formula
	desc "C++ program to generate waveform data and render waveform images from audio files"
	homepage "https://github.com/bbcrd/audiowaveform"
	url "https://github.com/bbcrd/audiowaveform/archive/1.0.10.tar.gz"
	sha256 "7d76a65962ec147befaad807aeae893a0d6f7f8443f9a5ecfe4776cbfc8cff37"

	depends_on "cmake" 
	depends_on "libmad"
	depends_on "libsndfile"
	depends_on "gd"
	if MacOS.version < :mavericks
		depends_on "boost" => "c++11"
	else
		depends_on "boost"
	end
	# depends_on "gmock"

	def install
		cmake_args = std_cmake_parameters.split
		cmake_args << "-DENABLE_TESTS=0"
		cmake_args << "-DCMAKE_C_COMPILER=/usr/bin/clang"
		cmake_args << "-DCMAKE_CXX_COMPILER=/usr/bin/clang++"
		cmake_args << "-DCMAKE_INSTALL_PREFIX=#{prefix}"

		mkdir "build" do
			system "cmake", "..", *cmake_args
			system "make", "install"
		end
	end

	test do
		# system "make", "test"
	end
end