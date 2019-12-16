#include <chrono>
#include <iostream>
#include <cstdint>
#include <thread>

#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/mavsdk.h>

using namespace mavsdk;
using namespace std::this_thread;
using namespace std::chrono;

#define ERROR_CONSOLE_TEXT "\033[31m" // Turn text on console red
#define TELEMETRY_CONSOLE_TEXT "\033[34m" // Turn text on console blue
#define NORMAL_CONSOLE_TEXT "\033[0m" // Restore normal console colour


void usage(std::string bin_name)
{
    std::cout << NORMAL_CONSOLE_TEXT << "Usage : " << bin_name << " <connection_url>" << std::endl
              << "Connection URL format should be :" << std::endl
              << " For TCP : tcp://[server_host][:server_port]" << std::endl
              << " For UDP : udp://[bind_host][:bind_port]" << std::endl
              << " For Serial : serial:///path/to/serial/dev[:baudrate]" << std::endl
              << "For example, to connect to the simulator use URL: udp://:14540" << std::endl;
}


int main(int argc, char** argv)
{
    Mavsdk dc;
    std::string  connection_url;
    ConnectionResult  connection_result;

    bool discover_system = false;

    if(argc == 2)
    {
        connection_url = argv[1];
        connection_result = dc.add_any_connection(connection_url);
    }
    else
    {
        usage(argv[0]);
        return 1;
    }
    
    if(connection_result != ConnectionResult::SUCCESS)
    {
        std::cout << "Connection Failed." <<std::endl;
        return 1;
    }

    System& system = dc.system();
    dc.register_on_discover([&discover_system](uint64_t uuid){
        std::cout << "Discovered system with UUID: " << uuid << std::endl;
        discover_system = true;
    });
    std::cout << "Waiting to discover system..." << std::endl;
    sleep_for(seconds(2));

    if(!discover_system)
    {
        std::cout << "Failed to discover system." << std::endl;
        return 1;
    }

    auto action = std::make_shared<Action>(system);
    auto telemetry = std::make_shared<Telemetry>(system);
    const Telemetry::Result set_rate_result = telemetry->set_rate_position(1.0);
    if (set_rate_result != Telemetry::Result::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT
                  << "Setting rate failed:" << Telemetry::result_str(set_rate_result)
                  << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }
    while (telemetry->health_all_ok() != true) {
        std::cout << "Vehicle is getting ready to arm" << std::endl;
        sleep_for(seconds(1));
    }


    std::cout << "Arming..." <<std::endl;
    const Action::Result action_result= action->arm();
    if(action_result != Action::Result::SUCCESS)
    {
        std::cout << "Failed to arm." <<std::endl;
        return 1;
    }
    std::cout << "Armed." << std::endl;


    std::cout << "Taking off..." <<std::endl;
    const Action::Result takeoff_result = action->takeoff();
    if(takeoff_result != Action::Result::SUCCESS)
    {
        std::cout << "Failed to take off." <<std::endl;
        return 1;
    }
    std::cout << "Take off successfully ." << std::endl;
    
    
    sleep_for(seconds(10));


    std::cout << "Mission starting..."<< std::endl;
    const Action::Result mission_result = action->goto_location( 47.398139363821485,8.5453846156597137,500,-60.0f);
    sleep_for(seconds(10));
    if(mission_result != Action::Result::SUCCESS)
    {
        std::cout << "Failed to finish the mission." <<std::endl;
        return 1;
    }
    std::cout << "Finish ." << std::endl;
    
    
    //sleep_for(seconds(10));
    

    std::cout << "Start to return home..." << std::endl;
    const Action::Result return_result = action->return_to_launch();
    if(return_result != Action::Result::SUCCESS)
    {
        std::cout << "Failed to return home." <<std::endl;
        return 1;
    }
    std::cout << "Finish ." << std::endl;


    sleep_for(seconds(30));

    
    std::cout << "Disarming..." <<std::endl;
    const Action::Result disarm_result = action->disarm();
    if(disarm_result != Action::Result::SUCCESS)
    {
        std::cout << "Failed to disarm." <<std::endl;
        return 1;
    }
    std::cout << "Disarmed ." << std::endl;
    
    return 0;

}