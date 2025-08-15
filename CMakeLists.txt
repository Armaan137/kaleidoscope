cmake_minimum_required(VERSION 3.28)
project(Kaleidoscope LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ZLIB REQUIRED) 
find_package(zstd REQUIRED)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")


add_executable(Kaleidoscope
    src/main.cpp
    src/lexer.cpp
)

target_include_directories(Kaleidoscope
    PRIVATE 
        ${LLVM_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

llvm_map_components_to_libnames(llvm_libs 
    support 
    core 
    irreader
)

target_link_libraries(Kaleidoscope 
    PRIVATE 
        ${llvm_libs}
)
