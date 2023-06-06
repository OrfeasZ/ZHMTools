cmake_minimum_required(VERSION 3.15)

set(NAVWEAKNESS_HEADERS
	Include/NavWeakness.h
	Include/Vec3.h
	Include/NavPower.h
)

set(NAVWEAKNESS_SOURCES
	Src/NavWeakness.cpp
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(NAVWEAKNESS_TYPE SHARED)
else()
	set(NAVWEAKNESS_TYPE STATIC)
endif()

add_library(NavWeakness ${NAVWEAKNESS_TYPE}
	${NAVWEAKNESS_HEADERS}
	${NAVWEAKNESS_SOURCES}
)

set_target_properties(NavWeakness PROPERTIES
	CXX_VISIBILITY_PRESET hidden
)

if((CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") AND (CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo))
	set(NAVWEAKNESS_COMPILE_FLAGS "/bigobj" "/Ob0" "/GL" "/O2")
	set(NAVWEAKNESS_LINK_FLAGS "/INCREMENTAL:NO" "/LTCG" "/OPT:REF" "/OPT:ICF" "/DYNAMICBASE" "/NXCOMPAT")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(NAVWEAKNESS_COMPILE_FLAGS "/bigobj")
	set(NAVWEAKNESS_LINK_FLAGS "")
else()
	set(NAVWEAKNESS_COMPILE_FLAGS "")
	set(NAVWEAKNESS_LINK_FLAGS "")
endif()

target_compile_options(NavWeakness PRIVATE ${NAVWEAKNESS_COMPILE_FLAGS})
target_link_options(NavWeakness PRIVATE ${NAVWEAKNESS_LINK_FLAGS})

target_compile_definitions(NavWeakness 
	PRIVATE NAVWEAKNESS_EXPORTS
)

target_include_directories(NavWeakness 
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Src
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include
)
