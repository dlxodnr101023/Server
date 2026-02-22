cmake_minimum_required(VERSION 3.10)
project(MyProject VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES src/main.cpp)

# 2. 패키지 찾기
find_package(fmt CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS asio)
find_package(OpenSSL REQUIRED)

add_executable(Servers ${SOURCES})

target_link_libraries(Servers PRIVATE 
    fmt::fmt 
    Boost::asio 
    OpenSSL::SSL 
    OpenSSL::Crypto
)

include_directories(include)