cmake_minimum_required(VERSION 3.20)
project(PXL_8 C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.5
)
FetchContent_MakeAvailable(raylib)

set(PLATFORM_SOURCES "")

# ---------- Icons ----------
if(WIN32)
    # Windows: icon embedded via the .rc file
    enable_language(RC)
    configure_file(resources/PXL-8.rc.in ${CMAKE_BINARY_DIR}/PXL-8.rc)
    list(APPEND PLATFORM_SOURCES ${CMAKE_BINARY_DIR}/PXL-8.rc)
elseif(APPLE)
    # macOS: app icon bundled in Contents/Resources
    set(MAC_ICON "${CMAKE_SOURCE_DIR}/resources/PXL-8_mac.icns")

    if(EXISTS "${MAC_ICON}")
        set_source_files_properties("${MAC_ICON}"
            PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
        list(APPEND PLATFORM_SOURCES "${MAC_ICON}")
    else()
        message(WARNING "macOS icon not found: ${MAC_ICON}")
    endif()
endif()

add_executable(PXL_8 main.cpp ${PLATFORM_SOURCES}
        sandboxGrid.h sandboxGrid.c
        fileLoading.hpp fileLoading.cpp
        messageBox.hpp
        Operand.hpp Operand.cpp
        Instruction.hpp
        Assembler.hpp Assembler.cpp
        VM.hpp VM.cpp)

target_link_libraries(PXL_8 PRIVATE raylib)
target_include_directories(PXL_8 PRIVATE ${CMAKE_SOURCE_DIR}/external)

# ---------- Per-platform linking ----------
if(WIN32)
    # MinGW: embed libgcc + libstdc++, hide console in Release
    target_link_options(PXL_8 PRIVATE -static-libgcc -static-libstdc++)
    target_link_options(PXL_8 PRIVATE $<$<CONFIG:Release>:-mwindows>)

    # libwinpthread can't be linked statically with CLion's MinGW,
    # so copy the DLL next to the exe after every build
    get_filename_component(MINGW_BIN_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)
    add_custom_command(TARGET PXL_8 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${MINGW_BIN_DIR}/libwinpthread-1.dll"
            $<TARGET_FILE_DIR:PXL_8>)

elseif(APPLE)
    # macOS: create a .app bundle
    find_package(Threads REQUIRED)
    target_link_libraries(PXL_8 PRIVATE Threads::Threads)

    set_target_properties(PXL_8 PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "PXL-8"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.aktr0s.pxl8"
        MACOSX_BUNDLE_ICON_FILE "PXL-8_mac.icns"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0.0"
    )

else()
    # Linux: embed libgcc + libstdc++, link threads
    find_package(Threads REQUIRED)
    target_link_libraries(PXL_8 PRIVATE Threads::Threads)
    target_link_options(PXL_8 PRIVATE -static-libgcc -static-libstdc++)
endif()