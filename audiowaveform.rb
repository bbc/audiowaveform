require 'formula'

class Audiowaveform < Formula
	desc "C++ program to generate waveform data and render waveform images from audio files"
	homepage "https://github.com/bbcrd/audiowaveform"
	url "https://github.com/bbcrd/audiowaveform/archive/1.0.10.tar.gz"
	version '1.0.10'
	sha256 "a33aa5e922cc61e0953d681178f2ac36536c7f98e962b86ee8285045f05b02b6"

	depends_on "cmake" 
	depends_on "libmad"
	depends_on "libsndfile"
	depends_on "gd"
	depends_on "boost" => "with-c++11"
	# depends_on "gmock"

	def install
		# ENV.deparallelize
		cmake_args = std_cmake_parameters.split
		cmake_args << "-D ENABLE_TESTS=0"
		cmake_args << "-D CMAKE_C_COMPILER=/usr/bin/clang"
		cmake_args << "-D CMAKE_CXX_COMPILER=/usr/bin/clang++"
		cmake_args << "-D CMAKE_INSTALL_PREFIX=#{prefix}"

		mkdir "build" do
			system "cmake", "..", *cmake_args
			system "make", "install"
		end
	end

	test do
		# system "make", "test"
	end
end