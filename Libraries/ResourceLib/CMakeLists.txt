cmake_minimum_required(VERSION 3.15)

set(RESOURCELIB_HEADERS
	Include/ResourceConverter.h
	Include/ResourceGenerator.h
	Include/ResourceLibCommon.h
	Include/ResourceLib.h
)

set(RESOURCELIB_SOURCES
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
	Src/ZHM/ZHMCustomProperties.cpp
	Src/ZHM/ZHMCustomProperties.h
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
	Src/ResourceConverterImpl.cpp
	Src/ResourceConverterImpl.h
	Src/ResourceGeneratorImpl.cpp
	Src/ResourceGeneratorImpl.h
	Src/ResourceLib.cpp
	Src/Resources.cpp
	Src/Resources.h
)

set(RESOURCELIB_SOURCES_HM3
	Src/Generated/HM3/ZHMEnums.cpp
	Src/Generated/HM3/ZHMEnums.h
	Src/Generated/HM3/ZHMGen.cpp
	Src/Generated/HM3/ZHMGen.h
	Src/Generated/HM3/ZHMProperties.cpp
	Src/Generated/HM3/ZHMProperties.h
	Include/ResourceLib_HM3.h
)

set(RESOURCELIB_SOURCES_HM2
	Src/Generated/HM2/ZHMEnums.cpp
	Src/Generated/HM2/ZHMEnums.h
	Src/Generated/HM2/ZHMGen.cpp
	Src/Generated/HM2/ZHMGen.h
	Src/Generated/HM2/ZHMProperties.cpp
	Src/Generated/HM2/ZHMProperties.h
	Include/ResourceLib_HM2.h
)

set(RESOURCELIB_SOURCES_HM2016
	Src/Generated/HM2016/ZHMEnums.cpp
	Src/Generated/HM2016/ZHMEnums.h
	Src/Generated/HM2016/ZHMGen.cpp
	Src/Generated/HM2016/ZHMGen.h
	Src/Generated/HM2016/ZHMProperties.cpp
	Src/Generated/HM2016/ZHMProperties.h
	Include/ResourceLib_HM2016.h
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(RESOURCELIB_TYPE SHARED)
else()
	set(RESOURCELIB_TYPE STATIC)
endif()

add_library(ResourceLib_HM3 ${RESOURCELIB_TYPE}
	${RESOURCELIB_HEADERS}
	${RESOURCELIB_SOURCES}
	${RESOURCELIB_SOURCES_HM3}
)

add_library(ResourceLib_HM2 ${RESOURCELIB_TYPE}
	${RESOURCELIB_HEADERS}
	${RESOURCELIB_SOURCES}
	${RESOURCELIB_SOURCES_HM2}
)

add_library(ResourceLib_HM2016 ${RESOURCELIB_TYPE}
	${RESOURCELIB_HEADERS}
	${RESOURCELIB_SOURCES}
	${RESOURCELIB_SOURCES_HM2016}
)

set_target_properties(ResourceLib_HM3 ResourceLib_HM2 ResourceLib_HM2016 PROPERTIES
	CXX_VISIBILITY_PRESET hidden
)

if((CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") AND (CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo))
	set(RESOURCELIB_COMPILE_FLAGS "/bigobj" "/Ob0" "/GL" "/O2")
	set(RESOURCELIB_LINK_FLAGS "/INCREMENTAL:NO" "/LTCG" "/OPT:REF" "/OPT:ICF" "/DYNAMICBASE" "/NXCOMPAT")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(RESOURCELIB_COMPILE_FLAGS "/bigobj")
	set(RESOURCELIB_LINK_FLAGS "")
else()
	set(RESOURCELIB_COMPILE_FLAGS "")
	set(RESOURCELIB_LINK_FLAGS "")
endif()

target_compile_options(ResourceLib_HM3 PRIVATE ${RESOURCELIB_COMPILE_FLAGS})
target_link_options(ResourceLib_HM3 PRIVATE ${RESOURCELIB_LINK_FLAGS})
target_compile_options(ResourceLib_HM2 PRIVATE ${RESOURCELIB_COMPILE_FLAGS})
target_link_options(ResourceLib_HM2 PRIVATE ${RESOURCELIB_LINK_FLAGS})

target_compile_definitions(ResourceLib_HM3 
	PRIVATE RESOURCELIB_EXPORTS ZHM_TARGET=3
)

target_compile_definitions(ResourceLib_HM2 
	PRIVATE RESOURCELIB_EXPORTS ZHM_TARGET=2
)

target_compile_definitions(ResourceLib_HM2016
	PRIVATE RESOURCELIB_EXPORTS ZHM_TARGET=2016
)

target_include_directories(ResourceLib_HM3 
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Src
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

target_include_directories(ResourceLib_HM2 
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Src
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

target_include_directories(ResourceLib_HM2016
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Src
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include
)
