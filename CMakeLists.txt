cmake_minimum_required(VERSION 3.1)

project (visualizer)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PULSE_SIMPLE libpulse-simple)
pkg_check_modules(RTAUDIO librtaudio)
pkg_check_modules(FFTW fftw3)

include_directories(${PULSE_SIMPLE_INCLUDE_DIRS} ${RTAUDIO_LIBRARIES})
add_compile_options(-Wall -std=c++11)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(main main.cc window.cc glwidget.cc audio.cc)
target_link_libraries(main ${PULSE_SIMPLE_LIBRARIES} ${FFTW_LIBRARIES})
target_link_libraries(main Qt5::OpenGL)

add_executable(dump audio.cc dump.cc)
target_link_libraries(dump ${RTAUDIO_LIBRARIES} ${FFTW_LIBRARIES})
