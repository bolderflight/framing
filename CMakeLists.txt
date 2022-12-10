cmake_minimum_required(VERSION 3.14)
# Project information
project(Framing
  VERSION 3.2.0
  DESCRIPTION "Framing"
  LANGUAGES CXX
)
# Grab checksum
include(FetchContent)
FetchContent_Declare(
	checksum
	GIT_REPOSITORY https://github.com/bolderflight/checksum.git
	GIT_TAG v3.2.0
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
target_include_directories(framing INTERFACE src/)

# Example and unit testing if this project is built separately
if (PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME)
	# Add the example target
	add_executable(framing_example examples/cmake/framing_example.cc)
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
	# Fetch google test
	include(FetchContent)
	FetchContent_Declare(
		googletest
		GIT_REPOSITORY https://github.com/google/googletest.git
		GIT_TAG release-1.12.1
	)
	FetchContent_MakeAvailable(googletest)
	enable_testing()
	include(GoogleTest)
	# Add the test target
	add_executable(framing_test tests/framing_test.cc)
	# Add the includes
	target_include_directories(framing_test PUBLIC 
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
	)
	# Link libraries to the test target
	target_link_libraries(framing_test
		PRIVATE 
			framing
			gtest_main 
			gtest
			gmock
	)
	# Discover unit tests
	gtest_discover_tests(framing_test)
endif()
