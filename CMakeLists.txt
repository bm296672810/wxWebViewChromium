#-- CMake version 3.2 or newer.
cmake_minimum_required(VERSION 3.2)

#-- Project name
project(mywbview)

set(CMAKE_CONFIGURATION_TYPES Debug Release)

set(CMAKE_BUILD_TYPE Debug)
set(CEF_ROOT
F:/tools/cef_binary_3.3538.1852.gcb937fc_windows32
)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")
find_package(CEF REQUIRED)

add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)

PRINT_CEF_CONFIG()

message("CEF_LIBCEF_DLL_WRAPPER_PATH:${CEF_LIBCEF_DLL_WRAPPER_PATH}")
message("CMAKE_CURRENT_BINARY_DIR:${CMAKE_CURRENT_BINARY_DIR}")
message("CEF_TARGET_OUT_DIR:${CEF_TARGET_OUT_DIR}")
set(WX_ROOT
C:/wxWidgets-3.0.1
)

set(WX_INCULDE_DIR
${WX_ROOT}/include
${WX_ROOT}/lib/vc_lib/mswu
)

#-- set include dir
set(INCLUDE_DIRS
${WX_INCULDE_DIR}
${CMAKE_CURRENT_SOURCE_DIR}
${CEF_ROOT}
)

INCLUDE_DIRECTORIES(${INCLUDE_DIRS})

include(cmake/files.cmake)

source_group(src FILES ${MY_SRCS})

set(MY_LIB_ROOT
lib/vc_lib
)

set(MY_LINK_DIRS
${WX_ROOT}/lib/vc_lib
${CEF_ROOT}
${CMAKE_CURRENT_BINARY_DIR}/libcef_dll_wrapper
)

link_directories(${MY_LINK_DIRS})

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")  
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF /SAFESEH:NO /NODEFAULTLIB:\"LIBCMT.lib\" /SUBSYSTEM:WINDOWS,5.01")

set(MY_DEFINE_MACRO
"-D_CRT_SECURE_NO_DEPRECATE=1"
"-D_CRT_NON_CONFORMING_SWPRINTFS=1"
"-D_SCL_SECURE_NO_WARNINGS=1"
"-D__WXMSW__"
"-DNDEBUG"
"-D_UNICODE"
"-D_WINDOWS"
"-DNOPCH"
"-DWIN32_LEAN_AND_MEAN"
"-DHAVE_STDINT_H"
"-DNOMINMAX"
"-D_CRT_SECURE_NO_WARNINGS"
"-D_WINSOCK_DEPRECATED_NO_WARNING"
#"-DGOOGLE_GLOG_DLL_DECL="
)
add_definitions(${MY_DEFINE_MACRO})

add_executable(${PROJECT_NAME} WIN32 ${MY_SRCS})

add_dependencies(${PROJECT_NAME} libcef_dll_wrapper)

if(MSVC)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /level='requireAdministrator' /uiAccess='false'" )
set(PDB_OUTPUT_DIRECTORY "${project_path}/Release")
set(PDB_NAME ${CMAKE_PROJECT_NAME}.pdb)
endif()

include(cmake/libs.cmake)

target_link_libraries(${PROJECT_NAME} ${OTHER_LINK_FLAGS} ${RELY_LIBS_WIN})
