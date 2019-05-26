# Copyright (c) 2017 The Bitcoin developers

cmake_minimum_required(VERSION 3.5)
project(BitcoinABC)

set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -fPIC")

# option(ENABLE_STATIC_LIBSTDCXX "Statically link libstdc++" ON)

# Cmake uses the CMAKE_BUILD_TYPE variable to select the build configuration.
# By default it supports more configurations that needed for Bitcoin ABC, and
# all the releases types set NDEBUG which is unwanted as it disables the assert
# completely.
# Remove the -DNDEBUG flag from the CFLAGS/CXXFLAGS in all the configurations
include(AddCompilerFlags)
option(ENABLE_STATIC_LIBSTDCXX "Statically link libstdc++" ON)

# Cmake uses the CMAKE_BUILD_TYPE variable to select the build configuration.
# By default it supports more configurations that needed for Bitcoin ABC, and
# all the releases types set NDEBUG which is unwanted as it disables the assert
# completely.
# Remove the -DNDEBUG flag from the CFLAGS/CXXFLAGS in all the configurations
include(AddCompilerFlags)
remove_compiler_flags(-DNDEBUG)

# Overrides the flags for the Debug build type
# This mimics the autotools behavior by setting the CFLAGS to '-g -O2`, which
# are not well suited for debugging.
# FIXME: update CFLAGS with better debug oriented optimization flags
set(CMAKE_C_FLAGS_DEBUG "-g -O2")

# Prefer -g3, defaults to -g if unavailable
add_cxx_compiler_flag_with_fallback(CMAKE_CXX_FLAGS_DEBUG -g3 -g)

# Prefer -Og, defaults to -O0 if unavailable
add_cxx_compiler_flag_with_fallback(CMAKE_CXX_FLAGS_DEBUG -Og -O0)

# Define the debugging symbols DEBUG and DEBUG_LOCKORDER when the Debug build
# type is selected.
string(APPEND CMAKE_CXX_FLAGS_DEBUG " -DDEBUG -DDEBUG_LOCKORDER")

# Enable statically linking libstdc++
if(ENABLE_STATIC_LIBSTDCXX)
	add_linker_flag(-static-libstdc++)
endif()

# Enable warning
add_c_compiler_flag(-Wnested-externs -Wstrict-prototypes)
add_compiler_flag(
	-Wall
	-Wextra
	-Wformat
	-Wvla
	-Wformat-security
	-Wcast-align
	-Wunused-parameter
	-Wmissing-braces
	# FIXME: Activating this flag cause cmake to fail on leveldb.
	# -Wthread-safety-analysis
    -Wshadow
)

# libtool style configure
add_subdirectory(config)

# libraries
add_subdirectory(secp256k1)

add_library(abc-consensus 
    amount.h
    amount.cpp
    hash.cpp
    hash.h
    prevector.h
    pubkey.cpp
    pubkey.h
    feerate.cpp
    feerate.h
    serialize.h
    tinyformat.h
    uint256.cpp
    uint256.h
    utilstrencodings.cpp
    utilstrencodings.h
    version.h
    compat/byteswap.h
    compat/endian.h
    crypto/common.h
    crypto/hmac_sha512.cpp
    crypto/hmac_sha512.h
    crypto/ripemd160.cpp
    crypto/ripemd160.h
    crypto/sha1.cpp
    crypto/sha1.h
    crypto/sha256.cpp
    crypto/sha256.h
    crypto/sha512.cpp
    crypto/sha512.h
    primitives/transaction.cpp
    primitives/transaction.h
    primitives/txid.h
    script/interpreter.cpp
    script/interpreter.h
    script/script.cpp
    script/script.h
    script/script_flags.h
    script/script_error.h
    script/sigencoding.h
    script/sigencoding.cpp
)

target_compile_definitions(abc-consensus PUBLIC HAVE_CONFIG_H)
target_include_directories(abc-consensus PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(abc-consensus secp256k1)

# Boost packages
set(BOOST_PACKAGES_REQUIRED chrono filesystem program_options thread)

function(prepend var prefix)
	set(listVar "")
	foreach(f ${ARGN})
		list(APPEND listVar "${prefix}${f}")
	endforeach(f)
	set(${var} "${listVar}" PARENT_SCOPE)
endfunction(prepend)

prepend(BOOST_LIBRARIES "Boost::" ${BOOST_PACKAGES_REQUIRED})

find_package(Boost 1.58 REQUIRED ${BOOST_PACKAGES_REQUIRED})

# Main Lib
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")
set(CMAKE_MACOSX_RPATH 1)

set (SOURCE-CONSENSUS export.cpp)
add_library(bitcoincash-consensus MODULE ${SOURCE-CONSENSUS})

target_compile_definitions(bitcoincash-consensus PUBLIC)
target_include_directories(bitcoincash-consensus PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(bitcoincash-consensus abc-consensus)