cmake_minimum_required(VERSION 3.15)

add_executable(NavTool 
	Src/NavTool.cpp
)

target_include_directories(NavTool PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/Src
)

target_link_libraries(NavTool PUBLIC
	NavWeakness
)

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_link_options(NavTool PRIVATE "-static")
endif()

# Copy over the NavWeakness binary so the executable can find it when running.
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	add_custom_command(TARGET NavTool POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		$<TARGET_FILE:NavWeakness> $<TARGET_FILE_DIR:NavTool>
	)
endif()