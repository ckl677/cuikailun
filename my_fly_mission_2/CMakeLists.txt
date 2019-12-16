cmake_minimum_required(VERSION 2.8.12)

project(my_fly_mission_1)

if(NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra -Werror")
else()
    add_definitions("-std=c++11 -WX -W2")
endif()

find_package(MAVSDK REQUIRED)

add_executable(my_fly_mission_2
    my_fly_mission_2.cpp
)

target_link_libraries(my_fly_mission_2
    MAVSDK::mavsdk_offboard
    MAVSDK::mavsdk_telemetry
    MAVSDK::mavsdk_action
    MAVSDK::mavsdk
)
