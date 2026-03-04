cmake_minimum_required(VERSION 3.8)

project("BGL")

# Add source files
file(GLOB_RECURSE PROJECT_SOURCES
    "${CMAKE_SOURCE_DIR}/classes/*.cpp"
    "${CMAKE_SOURCE_DIR}/classes/**/*.cpp"
    "${CMAKE_SOURCE_DIR}/utilClasses/*.cpp"
    "${CMAKE_SOURCE_DIR}/utilClasses/**/*.cpp"
)

# add main // windows is lame
if(WIN32)
    add_executable(BGL
    #WIN32
    "2ndmain.cpp"
    #"winmain.cpp"
    "mainmain.h"
    "lib/glad.c"
    ${PROJECT_SOURCES}
    )
else()
    add_executable(BGL
    "2ndmain.cpp"
    "mainmain.h"
    "lib/glad.c"
    ${PROJECT_SOURCES}
    )
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET BGL PROPERTY CXX_STANDARD 20)
endif()

# Find OpenGL and GLFW across all platforms
find_package(OpenGL REQUIRED)

if(WIN32)
    target_include_directories(BGL PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/include
    ${CMAKE_SOURCE_DIR}/lib
    )
    target_link_libraries(
        BGL
        ${CMAKE_SOURCE_DIR}/lib/lib-vc2022/glfw3.lib
        ${CMAKE_SOURCE_DIR}/lib/freetype.lib
        opengl32.lib
    )
    #add_custom_command(
    #    TARGET BGL POST_BUILD
    #    COMMAND ${CMAKE_COMMAND} -E copy_directory
    #            "${CMAKE_SOURCE_DIR}/assets"
    #            "$<TARGET_FILE_DIR:BGL>/assets"
    #)
endif()

add_custom_command(
        TARGET BGL POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/assets"
                "$<TARGET_FILE_DIR:BGL>/assets"
    )
add_custom_command(
        TARGET BGL POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/licenses"
                "$<TARGET_FILE_DIR:BGL>/licenses"
    )


target_include_directories(BGL PRIVATE 
    #${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_SOURCE_DIR}/classes
    ${CMAKE_SOURCE_DIR}/utilClasses
)
