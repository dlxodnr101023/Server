cmake_minimum_required(VERSION 3.10)
project(MyProject VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES 
    src/main.cpp
    src/password.cpp
    src/database/database.cpp
    src/database/AccountRepository.cpp
    src/packet.pb.cc
    src/mail.cpp
)

find_package(redis++ CONFIG REQUIRED)
find_package(mailio CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(OpenSSL REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(libpqxx CONFIG REQUIRED)
find_package(unofficial-argon2 CONFIG REQUIRED)

add_executable(Servers ${SOURCES})

target_include_directories(Servers PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/database
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(Servers PRIVATE 
    libpqxx::pqxx
    protobuf::libprotobuf
    unofficial::argon2::libargon2
    fmt::fmt
    OpenSSL::SSL
    mailio::mailio
    redis++::redis++_static
    ${Boost_LIBRARIES}
)