cmake_minimum_required(VERSION 3.8)
project(potdb)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES constants.cpp db.cpp errors.cpp tables.cpp)
add_executable(potdb ${SOURCE_FILES})