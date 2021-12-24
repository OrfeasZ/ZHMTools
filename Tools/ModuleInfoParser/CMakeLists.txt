cmake_minimum_required(VERSION 3.15)

add_executable(ModuleInfoParser 
	Src/ModuleInfoParser.cpp
	Src/External/pugixml.hpp
	Src/External/pugixml.cpp
	Src/External/pugiconfig.hpp
	Src/External/Hash.h
)

target_include_directories(ModuleInfoParser PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/Src
)

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_link_options(ModuleInfoParser PRIVATE "-static")
endif()
