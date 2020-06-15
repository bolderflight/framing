cmake_minimum_required(VERSION 3.13)
# Project information
project(Framing
  VERSION 1.0
  DESCRIPTION "Framing"
  LANGUAGES CXX
)
# Grab checksum
include(FetchContent)
FetchContent_Declare(
	checksum
	GIT_REPOSITORY 	git@gitlab.com:bolderflight/software/checksum.git
)
FetchContent_MakeAvailable(checksum)
# Add the library target
add_library(framing INTERFACE)
# Link libraries
target_link_libraries(framing
	INTERFACE
		checksum
)
# Setup include directories 
target_include_directories(framing INTERFACE include/)
# Example and unit testing if this project is built separately
if (PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME)
	# Add the example target
	add_executable(framing_example examples/framing_example.cc)
	# Add the includes
	target_include_directories(framing_example PUBLIC 
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
	)
	# Link libraries to the example target
	target_link_libraries(framing_example
		PRIVATE 
			framing
	)
endif()
