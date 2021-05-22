cmake_minimum_required(VERSION 3.12)


add_executable(ResourceTool 
	Src/ResourceTool.cpp
)

set_target_properties(ResourceTool PROPERTIES
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
	CXX_STANDARD 20
)

target_include_directories(ResourceTool PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/Src
)

target_link_libraries(ResourceTool PUBLIC
	ResourceLib
)

add_custom_command(TARGET ResourceTool POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:ResourceLib> $<TARGET_FILE_DIR:ResourceTool>
)