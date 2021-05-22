cmake_minimum_required(VERSION 3.12)

set(CODEGEN_SOURCES
	Src/CodeGen.cpp
	Src/CodeGen.h
)

add_executable(CodeGen 
	${CODEGEN_SOURCES}
)

set_target_properties(CodeGen PROPERTIES
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
	CXX_STANDARD 20
)

target_include_directories(CodeGen PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/Src
)