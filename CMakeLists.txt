cmake_minimum_required(VERSION 3.21)
project(SoccerAnimation LANGUAGES CXX)

if(NOT WIN32 OR NOT MSVC OR NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "Use an x64 MSVC toolchain to match the bundled FreeGLUT package.")
endif()

set(GLUT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/third_party/freeglut")
add_library(BundledFreeGLUT SHARED IMPORTED)
set_target_properties(BundledFreeGLUT PROPERTIES
    IMPORTED_IMPLIB "${GLUT_ROOT}/lib/freeglut.lib"
    IMPORTED_LOCATION "${GLUT_ROOT}/bin/freeglut.dll"
    INTERFACE_INCLUDE_DIRECTORIES "${GLUT_ROOT}/include")

add_executable(SoccerAnimation main.cpp)
target_compile_features(SoccerAnimation PRIVATE cxx_std_17)
target_compile_options(SoccerAnimation PRIVATE /W3)
target_link_libraries(SoccerAnimation PRIVATE BundledFreeGLUT opengl32 glu32)
set_target_properties(SoccerAnimation PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:SoccerAnimation>")
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT SoccerAnimation)
add_custom_command(TARGET SoccerAnimation POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "$<TARGET_FILE:BundledFreeGLUT>" "$<TARGET_FILE_DIR:SoccerAnimation>"
    VERBATIM)
