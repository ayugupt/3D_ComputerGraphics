cmake_minimum_required(VERSION 3.10)

project(VISUALIZATION VERSION 1.0)
configure_file(ProjectConfig.h.in ProjectConfig.h)

add_subdirectory(src)

add_executable(app main.cpp)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

target_include_directories(app PUBLIC ${PROJECT_BINARY_DIR}
                               "${PROJECT_SOURCE_DIR}/src"
                               ${GTKMM_INCLUDE_DIRS})

            
target_link_libraries(app PUBLIC ${GTKMM_LIBRARIES}
                                    srclib)


target_compile_options(app PUBLIC ${GTKMM_CFLAGS_OTHER})