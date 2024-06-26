#
# General
#

cmake_minimum_required(VERSION 3.5)

# Compiler - must be done before "project"

set(WARN_EXC "")

if(COMPILER STREQUAL "gcc")
	# enable these flags if you get tons of errors from boost...
	#set(WARN_EXC "-Wno-unused-local-typedefs -Wno-maybe-uninitialized")
	set(CMAKE_CXX_FLAGS_RELEASE "-flto")
	#set(FLTO_FLAGS "-flto") # TODO

	set(FLTO_FLAGS "")
	SET( CMAKE_CXX_COMPILER "g++" )
	SET( CMAKE_C_COMPILER "gcc" )
elseif(COMPILER STREQUAL "clang")
	set(FLTO_FLAGS "") # not supported by clang on most platforms

	SET( CMAKE_CXX_COMPILER "clang++" )
	SET( CMAKE_C_COMPILER "clang" )
elseif(COMPILER STREQUAL "native")
	message("Using native C/C++ compilers.")
else()
	message(FATAL_ERROR "Please specify -DCOMPILER=[gcc|clang]")
endif()

project(sca-toolsuite)

SET(VERSION_MAJOR "1")
SET(VERSION_MINOR "0")
SET(VERSION_PATCH "0")
SET(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

MESSAGE("")
MESSAGE("WELCOME")
MESSAGE("This is the Makefile for ${PROJECT_NAME} ${VERSION}")
MESSAGE("")
MESSAGE("READING CONFIGURATION...")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

# help
# TODO: $myproject from project()
# MESSAGE("This is the Makefile for ${MYPROJECT} (version ${VERSION}).")
# MESSAGE("Options: (-D...=..., cmake -L for details)")
# MESSAGE(" * COMPILER: ${COMPILER} (allowed: [gcc|clang])")

SET(COMPILER "" CACHE STRING "Compiler to use (allowed: [gcc|clang])")
option(WANT_IMAGEMAGICK "Compile files using imagemagick" OFF)

# testing
enable_testing ()

#
# Machine
#
#IF(WIN32)
#	SET(OS "win32")
#ELSE(WIN32)
#	SET(OS "linux")
#ENDIF(WIN32)
#
#SET(MY_HOST_X86 FALSE)
#SET(MY_HOST_X86_64 FALSE)
#
#IF(IS_X86)
#	SET(MY_HOST_X86 TRUE)
#	SET(MY_HOST_STR "x86")
#ELSEIF(IS_X86_64)
#	SET(MY_HOST_X86_64 TRUE)
#	SET(MY_HOST_STR "x86_64")
#ELSE(IS_X86)
#	MESSAGE("Can't identify target host. Assuming 32 bit platform.")
#	SET(MY_HOST_X86 TRUE)
#	SET(MY_HOST_STR "x86")
#ENDIF(IS_X86)
#
#MESSAGE("-- Target host is ${OS}, ${MY_HOST_STR}")

#include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/res/")
SET(src_base "${CMAKE_CURRENT_SOURCE_DIR}/src/")
SET(res_dir "${src_base}res/")

SET(general "${res_dir}/io.cpp" "${res_dir}/io.h" "${res_dir}/general.h" "${res_dir}/general.cpp" "${res_dir}/random.h")
SET(geometry "${res_dir}/geometry.h" "${res_dir}/geometry.cpp")
SET(eq_solver "${res_dir}/equation_solver.cpp" "${res_dir}/equation_solver.h" "${res_dir}/eqs_internal.h" "${res_dir}/eqs_functions.cpp" "${res_dir}/eqs_functions.h")
SET(ca "${res_dir}/ca.h" "${res_dir}/ca.cpp")
SET(ca_basics "${res_dir}/ca_basics.h")
SET(image "${res_dir}/image.h" "${res_dir}/image.cpp")
SET(print "${res_dir}/print.h")
SET(sim "${res_dir}/simulate.h" "${res_dir}/simulate.cpp")

function(cp_script script_name)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${script_name}
		${CMAKE_CURRENT_BINARY_DIR}/${script_name} COPYONLY)
endfunction()

# expects something like "${src_dir}/math.cpp"
function(compile cpp_name)
	get_filename_component(target_path "${cpp_name}" NAME_WE)
	add_executable("${target_path}" "${CMAKE_CURRENT_SOURCE_DIR}/${cpp_name}")
	target_link_libraries("${target_path}" res)
endfunction()

include_directories("${CMAKE_CURRENT_BINARY_DIR}src/") # for config.h
include_directories("${res_dir}")

#
# special headers
#

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in
${CMAKE_CURRENT_BINARY_DIR}/src/config.h)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in
${CMAKE_CURRENT_BINARY_DIR}/src/version.h)

# if(CUSTOM_INCLUDE_PATHS STREQUAL "")
# else()
# 	include_directories("${CUSTOM_INCLUDE_PATHS}")
# endif()
#link_directories("${ADD_LINK_DIRS}")

# INCLUDE (CheckIncludeFiles)


# add the command to generate the source code
add_custom_command (
	OUTPUT _sza2
	COMMAND echo ${VERSION} > ${CMAKE_CURRENT_BINARY_DIR}/sza2
)

add_custom_target (
	sza2
	DEPENDS _sza2
)

MESSAGE("CHECKING FOR LIBRARIES...")

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED OFF)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.46.0
	REQUIRED
	COMPONENTS graph)

find_package(Qt5
	COMPONENTS Widgets)
if(Qt5_FOUND)
	set(BUILD_QT_GUI true)
else(Qt5_FOUND)
	set(BUILD_QT_GUI false)
endif(Qt5_FOUND)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/usr/CMakeLists.txt")
	set(BUILD_USR_DIR true)
else()
	set(BUILD_USR_DIR false)
endif()

find_package(ImageMagick COMPONENTS MagickCore Magick++)
if(ImageMagick_FOUND AND WANT_IMAGEMAGICK)
	set(BUILD_IMG_MAGICK true)
else()
	set(BUILD_IMG_MAGICK false)
endif()

#if(Boost_FOUND)
#	include_directories(${Boost_INCLUDE_DIRS})
#	add_executable(progname file1.cxx file2.cxx)
#	target_link_libraries(progname ${Boost_LIBRARIES})
#endif()

#
# Compile
#

#if(LFTO STREQUAL "1")

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
	message(FATAL_ERROR "Please set -DCMAKE_BUILD_TYPE=[Debug|Release]")
endif()
endif()

# FLAGS
# the order of -Wall -Wno-unknown-pragmas is important
# TODO: -Werror
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-implicit-fallthrough ${WARN_EXC} -std=c++11 -pipe -march=native -mtune=native")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 ${FLTO_FLAGS}")

# add profiling for gcc:
if(COMPILER STREQUAL "gcc")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
endif()

MESSAGE("READING SUB-DIRECTORIES...")
add_subdirectory(src)
add_subdirectory(doc)
#find_package(Doxygen) # TODO: wrong place

#target_link_libraries(calc ...)


#add_test (test sh ${CMAKE_CURRENT_BINARY_DIR}/src/test.sh)
#TODO: use cmake instead of sh script?

#
# CPack Stuff
#

SET(MY_NAME_CAP ${PROJECT_NAME})
#SET(MY_NAME_LOW ${PROJECT_NAME}) # (TODO: tolower)
STRING(TOLOWER ${MY_NAME_CAP} MY_NAME_LOW)
#SET(MY_PACKAGE_FILE_NAME "${MY_NAME_LOW}-${VERSION}-${OS}-${MY_HOST_STR}")
SET(MY_PACKAGE_FILE_NAME "${MY_NAME_LOW}-${VERSION}")
SET(MY_SRC_PACKAGE_FILE_NAME "${MY_NAME_LOW}-${VERSION}")

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${MY_NAME_CAP} - sca cellular automata toolsuite")
IF(WIN32)
SET(CPACK_PACKAGE_EXECUTABLES "${MY_NAME_LOW}.exe")
ELSE(WIN32)
SET(CPACK_PACKAGE_EXECUTABLES "${MY_NAME_LOW}")
SET(CPACK_STRIP_FILES "bin/${MY_NAME_LOW}") # TODO: bin?
ENDIF(WIN32)
SET(CPACK_PACKAGE_FILE_NAME ${MY_PACKAGE_FILE_NAME})
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "${MY_SRC_PACKAGE_FILE_NAME}")
SET(CPACK_PACKAGE_VENDOR "${MY_NAME_CAP} Developers")
SET(CPACK_PACKAGE_VERSION ${VERSION})
SET(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.txt")
SET(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
SET(CPACK_SOURCE_GENERATOR "TBZ2")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${MY_SRC_PACKAGE_FILE_NAME}")
INCLUDE(CPack)

#
# Summary
#

MESSAGE("")
MESSAGE("SUMMARY:")
MESSAGE(" * Project: ${PROJECT_NAME} ${VERSION}")
MESSAGE(" * Compilers Used: ${COMPILER} (${CMAKE_CXX_COMPILER}, ${CMAKE_CC_COMPILER})")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(MSG_BUILD_TYPE_FLAG ${CMAKE_CXX_FLAGS_DEBUG})
else()
	set(MSG_BUILD_TYPE_FLAG ${CMAKE_CXX_FLAGS_RELEASE})
endif()
MESSAGE(" * Build Type: ${CMAKE_BUILD_TYPE} (${MSG_BUILD_TYPE_FLAG})")

if(BUILD_QT_GUI)
	set(MSG_QT_FOUND "Yes")
else(BUILD_QT_GUI)
	set(MSG_QT_FOUND "No - install Qt if you want")
endif(BUILD_QT_GUI)
MESSAGE(" * Build Qt GUI: ${MSG_QT_FOUND}")

if(BUILD_USR_DIR)
	set(MSG_USR_FOUND "Yes - Found source files")
else(BUILD_USR_DIR)
	set(MSG_USR_FOUND "No - No usr dir in /src")
endif(BUILD_USR_DIR)
MESSAGE(" * Build USR DIR: ${MSG_USR_FOUND}")

if(BUILD_IMG_MAGICK)
	set(MSG_IM_FOUND "Yes - Found Magick++")
else(BUILD_IMG_MAGICK)
	set(MSG_IM_FOUND "No - No Magick++ found")
endif(BUILD_IMG_MAGICK)
MESSAGE(" * Build ImageMagick components: ${MSG_IM_FOUND}")

if(DOXYGEN_FOUND)
	set(MSG_DOXY_FOUND "No - Type make doc if you want")
else(DOXYGEN_FOUND)
	set(MSG_DOXY_FOUND "No - No Doxygen found")
endif(DOXYGEN_FOUND)
MESSAGE(" * Building Doc: ${MSG_DOXY_FOUND}")

MESSAGE(" * Executing Tests: No - Type make test if you want")
MESSAGE("")
MESSAGE("Now, type make and pray...")
MESSAGE("")

