cmake_minimum_required(VERSION 3.15)

add_executable(ResourceTool 
	Src/ResourceTool.cpp
)

target_include_directories(ResourceTool PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/Src
)

target_link_libraries(ResourceTool PUBLIC
	ResourceLib_HM3
	ResourceLib_HM2
)

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_link_options(ResourceTool PRIVATE "-static")
endif()

# Copy over the ResourceLib binaries so the executable can find them when running.
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	add_custom_command(TARGET ResourceTool POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		$<TARGET_FILE:ResourceLib_HM3> $<TARGET_FILE_DIR:ResourceTool>
	)

	add_custom_command(TARGET ResourceTool POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		$<TARGET_FILE:ResourceLib_HM2> $<TARGET_FILE_DIR:ResourceTool>
	)
endif()