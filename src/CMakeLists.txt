add_subdirectory(model)
add_subdirectory(libs)

add_library(srclib MainWindow.cpp MainWindow.h Canvas.cpp Canvas.h Camera.cpp Camera.h MatrixFunctions.cpp MatrixFunctions.h)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

target_include_directories(srclib PUBLIC ${GTKMM_INCLUDE_DIRS}
                                         "${PROJECT_SOURCE_DIR}/src/libs"
                                         "${PROJECT_SOURCE_DIR}/src/model")
target_link_libraries(srclib PUBLIC ${GTKMM_LIBRARIES}
                                    modellib
                                    libs)
target_compile_options(srclib PUBLIC ${GTKMM_CFLAGS_OTHER})
