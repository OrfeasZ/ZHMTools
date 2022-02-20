cmake_minimum_required(VERSION 3.15)

add_executable(PropertyBundler 
	Src/PropertyBundler.cpp
	Src/External/Hash.h
 "Src/BinaryStreamWriter.h")

target_include_directories(PropertyBundler PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/Src
)

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_link_options(PropertyBundler PRIVATE "-static")
endif()
