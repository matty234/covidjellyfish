cmake_minimum_required(VERSION 3.1)
project(covidjellyfish)


set(CMAKE_CXX_STANDARD 11)

find_package(PkgConfig REQUIRED)

pkg_check_modules(JELLYFISH_PKG REQUIRED IMPORTED_TARGET jellyfish-2.0)

find_package( Boost 1.75 COMPONENTS system iostreams REQUIRED )
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

add_executable(covidjellyfish src/query_parameters.hpp src/sequence_mers.hpp src/query_per_sequence.cc)

target_link_libraries(covidjellyfish PUBLIC PkgConfig::JELLYFISH_PKG ${Boost_LIBRARIES})
