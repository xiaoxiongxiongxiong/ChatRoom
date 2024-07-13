#include <iostream>
#include "log.h"
#include "ChatRoomServerImpl.h"

int main(int argc, char * argv[])
{
    const char * path = "server.log";
    if (0 != log_msg_init(path, LOG_LEVEL_DEBUG))
    {
        std::cout << "log init failed" << std::endl;
        return EXIT_FAILURE;
    }
    atexit(log_msg_uninit);

    auto & server = CEpollServerImpl::getInstance();
    server.create("0.0.0.0", 31000, 32);

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    return EXIT_SUCCESS;
}
