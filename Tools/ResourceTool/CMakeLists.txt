cmake_minimum_required(VERSION 3.12)

set(RESOURCETOOL_SOURCES
	Src/External/simdjson.cpp
	Src/External/simdjson.h
	Src/Util/Base64.h
	Src/Util/BinaryStreamReader.h
	Src/Util/BinaryStreamWriter.h
	Src/Util/PortableIntrinsics.h
	Src/ZHM/Hash.h
	Src/ZHM/TArray.h
	Src/ZHM/THashMap.h
	Src/ZHM/TPair.h
	Src/ZHM/ZHMCustomTypes.cpp
	Src/ZHM/ZHMCustomTypes.h
	Src/ZHM/ZHMPrimitives.cpp
	Src/ZHM/ZHMPrimitives.h
	Src/ZHM/ZHMSerializer.cpp
	Src/ZHM/ZHMSerializer.h
	Src/ZHM/ZHMTypeInfo.cpp
	Src/ZHM/ZHMTypeInfo.h
	Src/ZHM/ZRepositoryID.cpp
	Src/ZHM/ZRepositoryID.h
	Src/ZHM/ZString.cpp
	Src/ZHM/ZString.h
	Src/ZHM/ZVariant.cpp
	Src/ZHM/ZVariant.h
	Src/main.cpp
	Src/ResourceConverter.cpp
	Src/ResourceConverter.h
	Src/ResourceGenerator.h
	Src/Resources.cpp
	Src/Resources.h
)

set(RESOURCETOOL_SOURCES_HM3
	Src/Generated/ZHMEnums.cpp
	Src/Generated/ZHMEnums.h
	Src/Generated/ZHMGen.cpp
	Src/Generated/ZHMGen.h
	Src/Generated/ZHMProperties.cpp
	Src/Generated/ZHMProperties.h
)

add_executable(ResourceTool 
	${RESOURCETOOL_SOURCES}
	${RESOURCETOOL_SOURCES_HM3}
)

set_target_properties(ResourceTool PROPERTIES
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
	CXX_STANDARD 20
)

target_compile_options(ResourceTool
	PUBLIC "/bigobj"
)

target_include_directories(ResourceTool PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/Src
)