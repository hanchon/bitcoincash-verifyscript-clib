# Copyright (c) 2017 The Bitcoin developers

cmake_minimum_required(VERSION 3.5)
project(BitcoinABC-Consensus)

# Add path for custom modules
set(CMAKE_MODULE_PATH
	${CMAKE_MODULE_PATH}
	${CMAKE_CURRENT_SOURCE_DIR}/cmake
)
# Default to RelWithDebInfo configuration
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
		"Select the configuration for the build" FORCE)
endif()

add_subdirectory(src)