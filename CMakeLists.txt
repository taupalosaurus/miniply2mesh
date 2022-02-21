cmake_minimum_required(VERSION 3.8)
PROJECT(ply2meshb)


set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules ${CMAKE_MODULE_PATH})
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(BIN_INSTALL_DIR "./install")
set(CMAKE_CXX_STANDARD 14)

if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.9)
endif()

FIND_PACKAGE(LibMeshb)
if(LIBMESHB_FOUND)
  include_directories(${LIBMESHB_INCLUDE_DIR})
  set (L_LIBRARIES ${LIBMESHB_LIBRARIES} ${L_LIBRARIES})
endif()

project(miniply LANGUAGES C CXX)

file(GLOB_RECURSE miniply_sources "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/src/*.hpp")

file(GLOB_RECURSE reader_sources  "${PROJECT_SOURCE_DIR}/src/*.c")


if(WIN32)
    add_definitions("-D_USE_MATH_DEFINES")
endif()


add_executable(${PROJECT_NAME} ${miniply_sources})
target_link_libraries(${PROJECT_NAME} ${L_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/external)

add_executable(reader ${reader_sources})
target_link_libraries(reader ${L_LIBRARIES})
target_include_directories(reader PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(reader PUBLIC ${PROJECT_SOURCE_DIR}/external)

install(TARGETS ${PROJECT_NAME} DESTINATION ${BIN_INSTALL_DIR})