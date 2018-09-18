#-- CMake version 3.2 or newer.
cmake_minimum_required(VERSION 3.2)

#-- Project name
project(mywbview)

set(CMAKE_CONFIGURATION_TYPES Debug Release)

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
)

INCLUDE_DIRECTORIES(${INCLUDE_DIRS})

set(MY_SRCS
src/myApp.h
src/myApp.cpp
src/code.h
src/code.cpp
src/MyEvent.h
src/MyEvent.cpp
src/webview_chromium.h
src/webview_chromium.cpp
src/webview/MyCefApp.h
src/webview/MyCefApp.cpp
src/webview/MyCefV8Handler.h
src/webview/MyCefV8Handler.cpp
src/webview/MyCefMessage.h
)

source_group(src FILES ${MY_SRCS})

set(MY_LIB_ROOT
lib/vc_lib
)

set(MY_LINK_DIRS
${WX_ROOT}/lib/vc_lib
${MY_LIB_ROOT}/cef
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

ADD_EXECUTABLE(${PROJECT_NAME} WIN32 ${MY_SRCS})

if(MSVC)
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /level='requireAdministrator' /uiAccess='false'" )
set(PDB_OUTPUT_DIRECTORY "${project_path}/Release")
set(PDB_NAME ${CMAKE_PROJECT_NAME}.pdb)
endif()

set(RELAY_WX_LIBS
wxbase30d.lib
wxmsw30d_core.lib
wxmsw30d_webview.lib
)

set(RELY_LIBS_WIN
${RELAY_WX_LIBS}
libcef.lib
libcef_dll_wrapper.lib
kernel32.lib
user32.lib
gdi32.lib
comdlg32.lib
winspool.lib
winmm.lib
shell32.lib
comctl32.lib
ole32.lib
oleaut32.lib
uuid.lib
rpcrt4.lib
advapi32.lib
wsock32.lib
wininet.lib
)

target_link_libraries(${PROJECT_NAME} ${OTHER_LINK_FLAGS} ${RELY_LIBS_WIN})

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/Release)