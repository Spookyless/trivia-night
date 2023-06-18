cmake_minimum_required(VERSION 3.22)
project(trivia_night)

set(CMAKE_CXX_STANDARD 17)

find_package(CURL REQUIRED)

add_executable(trivia_night main.cpp Trivia.cpp)

add_compile_options(-static-libstdc++ -static-libstdc)

target_include_directories(trivia_night PRIVATE ${CURL_INCLUDE_DIRS})
target_include_directories(trivia_night PRIVATE nlohmann.json/include)
target_include_directories(trivia_night PRIVATE lamarrr.chalk/include)
target_include_directories(trivia_night PRIVATE aelliixx.cpp-inquirer/include)

target_link_libraries(trivia_night PRIVATE CURL::libcurl)
