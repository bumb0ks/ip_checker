
#include "pinger/pinger.h"
#include "ip_list/ip_list.h"

#include <boost/thread.hpp>


#include <iostream>
#include <chrono>
#include <utility>
#include <string>

#include "experimental/data_generator.h"
#include "experimental/threaded_ioc_manager.h"

void run(char** ips)
{
    string first_ip(ips[1]);
    string last_ip(ips[2]);

    IPList list(first_ip, last_ip);

    boost::shared_ptr<data_generator<IPList>> g(new data_generator(list));

    threaded_io_service_manager<pinger, data_generator<IPList> > manager(3, 96);
    manager.set_data_generator(g);
    manager.run();
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cout << "Incorrect use.\nExample:\n\tip_checker.exe <First_IP> <Last_IP>" << std::endl;
        return -1;
    }
    
    std::chrono::time_point now = std::chrono::steady_clock::now();
    
    run(argv);

    auto t = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - now).count();
    cout << "Time spent: " << t / 1000 << "." << t % 1000 << "ms";

    return 0;
}