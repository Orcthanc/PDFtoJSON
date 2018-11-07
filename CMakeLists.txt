cmake_minimum_required (VERSION 3.0)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

project (PDFtoJSON)

add_subdirectory(external/PDF-Writer)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE PDFtoJSON_SOURCES "src/*.cpp")

add_executable (PDFtoJSON ${PDFtoJSON_SOURCES})

target_include_directories(PDFtoJSON PUBLIC "src" "external/PDF-Writer" )
target_link_libraries(PDFtoJSON PRIVATE LibAesgm LibJpeg Zlib LibTiff FreeType LibPng PDFWriter)
