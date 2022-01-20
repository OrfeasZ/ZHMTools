cmake_minimum_required(VERSION 3.15)

add_library(CodeGenLib SHARED
	Src/CodeGenLib.cpp
	Src/Utils.h 
	Src/Utils.cpp
	Src/ZHMReflection.h
	Src/ZHMCommon.h
	Src/CodeGen.h
	Src/CodeGen.cpp
	Src/HMAData.cpp
	Src/HMAData.h
)

target_include_directories(CodeGenLib PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/Src
)
