cmake_minimum_required(VERSION 3.15)

set(CODEGEN_SOURCES
	Src/CodeGen.cpp
)

add_executable(CodeGen 
	${CODEGEN_SOURCES}
)

target_include_directories(CodeGen PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/Src
)

add_dependencies(CodeGen CodeGenLib)

# Copy over the CodeGenLib binaries so the executable can find them when running.
add_custom_command(TARGET CodeGen POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:CodeGenLib> $<TARGET_FILE_DIR:CodeGen>
)