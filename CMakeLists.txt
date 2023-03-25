cmake_minimum_required(VERSION 3.2)

project(hl_mai_lab_02 C CXX)

SET (EXAMPLE_BINARY "hl_mai_lab_02")

find_package(OpenSSL)
find_package(Threads)
find_package(ZLIB)
find_package(Poco REQUIRED COMPONENTS Foundation Util Net XML JSON Crypto NetSSL Data)

if(NOT ${Poco_FOUND})
    message(FATAL_ERROR "Poco C++ Libraries not found.")
endif()

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${Poco_INCLUDE_DIRS})

add_executable(${EXAMPLE_BINARY} main.cpp 
                                 config/config.cpp 
                                 database/database.cpp
                                 database/user.cpp
                                 database/message.cpp)


target_compile_options(${EXAMPLE_BINARY} PRIVATE -Wall -Wextra -pedantic -Werror )
target_link_libraries(${EXAMPLE_BINARY} PRIVATE  ${CMAKE_THREAD_LIBS_INIT} ${Poco_LIBRARIES} PocoDataMySQL)



