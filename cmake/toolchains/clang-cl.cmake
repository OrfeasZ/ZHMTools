set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR AMD64)

set(xwin "$ENV{XWIN_SPLAT_DIR}")
if (NOT xwin)
  get_filename_component(xwin "../../.xwin/splat" ABSOLUTE)
endif ()
if (NOT EXISTS "${xwin}/crt/include")
  message(FATAL_ERROR "no Windows SDK at ${xwin} -- run scripts/fetch-xwin.sh")
endif ()

set(CMAKE_C_COMPILER clang-cl)
set(CMAKE_CXX_COMPILER clang-cl)
set(CMAKE_C_COMPILER_TARGET x86_64-pc-windows-msvc)
set(CMAKE_CXX_COMPILER_TARGET x86_64-pc-windows-msvc)
set(CMAKE_LINKER lld-link)
set(CMAKE_AR llvm-lib)
set(CMAKE_RC_COMPILER llvm-rc)
set(CMAKE_MT llvm-mt)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Emit BOTH CodeView (for PDBs distributed to Windows users) and DWARF (so GDB
# on Linux can read symbols directly out of the .dll when debugging the game
# under Proton). clang-cl with `/Z7 /clang:-gdwarf-4` produces both `.debug$S`
# (CodeView) and `.debug_*` (DWARF) sections in the .obj; lld-link consumes
# CodeView to generate a .pdb and keeps the DWARF sections in the output .dll.
#
# We pin the CMake debug-format policy to "Embedded" (/Z7) so the format is
# explicit and PDBs are linker-generated, then layer DWARF on top.
set(CMAKE_POLICY_DEFAULT_CMP0141 NEW)
set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "Embedded")
set(_zknt_dwarf_flags "/clang:-gdwarf-4 /clang:-fstandalone-debug")
set(CMAKE_C_FLAGS_DEBUG_INIT "${_zknt_dwarf_flags}")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "${_zknt_dwarf_flags}")
set(CMAKE_C_FLAGS_RELWITHDEBINFO_INIT "${_zknt_dwarf_flags} -O2")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "${_zknt_dwarf_flags} -O2")

foreach (d crt/include sdk/include/ucrt sdk/include/um sdk/include/shared)
  string(APPEND inc " /imsvc${xwin}/${d}")
endforeach ()
set(CMAKE_C_FLAGS_INIT "${inc} -Wno-unused-command-line-argument")
set(CMAKE_CXX_FLAGS_INIT "${inc} -Wno-unused-command-line-argument /EHsc")

foreach (d crt/lib/x86_64 sdk/lib/um/x86_64 sdk/lib/ucrt/x86_64)
  string(APPEND lib " /libpath:${xwin}/${d}")
endforeach ()

set(CMAKE_EXE_LINKER_FLAGS_INIT "${lib}")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "${lib}")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "${lib}")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
