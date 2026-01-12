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
	Src/RTTI/Image.h
	Src/RTTI/Image.cpp
	Src/RTTI/RttiParser.h
	Src/RTTI/RttiParser.cpp
	Src/RTTI/VTableFinder.h
	Src/RTTI/VTableFinder.cpp
)

target_include_directories(CodeGenLib PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/Src
)

target_link_libraries(CodeGenLib PUBLIC
	dbghelp
)