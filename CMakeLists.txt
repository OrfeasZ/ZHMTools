cmake_minimum_required(VERSION 3.15)

if(POLICY CMP0091)
	cmake_policy(SET CMP0091 NEW)
endif()

project("ZHMTools")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 20)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(VCPKG_CRT_LINKAGE "static")
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

	string(REPLACE "/INCREMENTAL" "" CMAKE_SHARED_LINKER_FLAGS_RELEASE ${CMAKE_SHARED_LINKER_FLAGS_RELEASE})
	string(REPLACE "/INCREMENTAL" "" CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO ${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO})

	string(REPLACE "/MDd" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
	string(REPLACE "/O2" "" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
	string(REPLACE "/O2" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
	string(REPLACE "/Ob1" "" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
	string(REPLACE "/Ob1" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
elseif(DEFINED LIBCPP_VERSION)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nostdinc++ -I/usr/include/c++/${LIBCPP_VERSION} -I/usr/include/x86_64-linux-gnu/c++/${LIBCPP_VERSION}")
endif()

if(EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS_RELEASE "")
endif()

add_subdirectory("Libraries/ResourceLib")

if(NOT EMSCRIPTEN)
	add_subdirectory("Tools/ResourceTool")

	add_subdirectory("Libraries/NavWeakness")
	add_subdirectory("Tools/NavTool")

	add_subdirectory("Tools/ModuleInfoParser")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	add_subdirectory("Libraries/CodeGenLib")
	add_subdirectory("Tools/CodeGen")
endif()
