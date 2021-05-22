cmake_minimum_required(VERSION 3.8)

project("ZHMTools")

add_subdirectory("Libraries/ResourceLib")
add_subdirectory("Libraries/CodeGenLib")

add_subdirectory("Tools/ResourceTool")
add_subdirectory("Tools/CodeGen")
