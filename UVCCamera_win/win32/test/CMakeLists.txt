cmake_minimum_required(VERSION 2.8)
project(UVCCameraTest)

set(CMAKE_CXX_STANDARD 11)

add_executable(${PROJECT_NAME} main.cc)

if(APPLE)
        set_target_properties(${PROJECT_NAME} PROPERTIES
                INSTALL_RPATH "@executable_path;@executable_path/../lib"
                BUILD_WITH_INSTALL_RPATH TRUE
        )
        find_library(CoreFoundation_LIB CoreFoundation)
        find_library(IOKit_LIB IOKit)
        find_library(Security_LIB Security)
        target_link_libraries(${PROJECT_NAME} PUBLIC UVCCamera
                ${IOKit_LIB}
                ${CoreFoundation_LIB}
                ${Security_LIB}
        )
else()
        target_link_libraries(${PROJECT_NAME} PUBLIC UVCCamera)
endif(APPLE)

