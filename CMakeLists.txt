set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
cmake_minimum_required(VERSION 3.8)

project("gex2ps1soundexporter")

set(SOURCE_FILES_EXPORTER
  ${CMAKE_CURRENT_SOURCE_DIR}/src/FLACExport.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Globals.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SoundExporter.cpp
)

set(HEADER_FILES_EXPORTER
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Constants.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/FLACExport
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Globals.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/PCMStructs.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SoundExporter.h
)

add_executable(gex2ps1soundexporter ${SOURCE_FILES_EXPORTER} ${HEADER_FILES_EXPORTER})

set_property(TARGET gex2ps1soundexporter PROPERTY CXX_STANDARD 20)

target_include_directories(gex2ps1soundexporter PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")


find_package(FLAC REQUIRED)

find_path(FLAC_INCLUDE_DIRS_THIS_ONE NAMES encoder.h HINTS FLAC++)
find_library(FLAC_LIBRARIES NAMES FLAC)

target_include_directories(gex2ps1soundexporter PRIVATE "/usr/include/FLAC++/")
target_link_libraries(gex2ps1soundexporter PRIVATE "/usr/lib/libFLAC++.so")

message(STATUS "FLAC_INCLUDE_DIRS: ${FLAC_INCLUDE_DIRS_THIS_ONE}")
message(STATUS "FLAC_LIBRARIES: ${FLAC_LIBRARIES}")


install(TARGETS gex2ps1soundexporter DESTINATION bin)
