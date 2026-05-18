cmake_minimum_required(VERSION 3.8)

project("BGL")

# You can change this to a different C++ version but you might break project. Idk
set(CMAKE_CXX_STANDARD 20)

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
    "lib/shared/glad.c"
    ${PROJECT_SOURCES}
    )
elseif(UNIX AND NOT APPLE)
    add_executable(BGL
    "2ndmain.cpp"
    "mainmain.h"
    "lib/shared/glad.c"
    ${PROJECT_SOURCES}
    )
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET BGL PROPERTY CXX_STANDARD 20)
endif()

# lol no this breaks the linux build # Find OpenGL and GLFW across all platforms
# find_package(OpenGL REQUIRED)

if(WIN32)
    find_package(OpenGL REQUIRED)
    target_include_directories(BGL PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/shared/include
        ${CMAKE_SOURCE_DIR}/lib/shared
        ${CMAKE_SOURCE_DIR}/lib/windows
    )
    target_link_libraries(
        BGL
        ${CMAKE_SOURCE_DIR}/lib/windows/lib-vc2022/glfw3.lib
        ${CMAKE_SOURCE_DIR}/lib/shared/freetype.lib
        opengl32.lib
    )
elseif(UNIX AND NOT APPLE)

    find_package(OpenGL REQUIRED)

    #add_subdirectory(external/glfw)

    target_include_directories(BGL PRIVATE
       #${GLFW_INCLUDE_DIRS}
       ${CMAKE_SOURCE_DIR}/lib/shared/include
       ${CMAKE_SOURCE_DIR}/lib/shared
       ${CMAKE_SOURCE_DIR}/lib/linux
       #${CMAKE_SOURCE_DIR}/lib/linux/glfw

    )

    target_link_libraries(BGL PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/linux/glfw/libglfw3.a
        ${CMAKE_SOURCE_DIR}/lib/linux/freetype/libfreetype.a
        OpenGL::GL
        dl
        pthread
        X11
        Xrandr
        Xi
    )
    # to get glfw on your linux pc
    # sudo apt install libglfw3-dev

    #find_package(PkgConfig REQUIRED)

    #throws error here
    #pkg_check_modules(GLFW REQUIRED glfw3)


    #ignore past here because it does not get that far
    #target_include_directories(BGL PRIVATE ${GLFW_INCLUDE_DIRS})
    #target_link_libraries(BGL PRIVATE ${GLFW_LIBRARIES})

    #target_include_directories(BGL PRIVATE
    #    ${CMAKE_SOURCE_DIR}/lib/shared/include
    ##    ${CMAKE_SOURCE_DIR}/lib/shared
     #   ${CMAKE_SOURCE_DIR}/lib/linux
    #)

    #target_link_libraries(
    #    BGL
    #    ${CMAKE_SOURCE_DIR}/lib/linux/glfw/libglfw.so
    #    ${CMAKE_SOURCE_DIR}/lib/shared/freetype.lib
        #opengl32.lib
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
