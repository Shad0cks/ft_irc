#include "includes.hpp"
#include <stdio.h>
#include <stdlib.h>

int checkport(std::string   port)
{
    for (int i = 0; i < port.size(); i++)
    {
        if (!std::isdigit (port[i]))
        {
            std::cout << "The port is not a number" << std::endl;
            return (0);
        }
    }
    int i = std::stoi(port);
    if (i < 1024 || i > 65535)
    {
        std::cout << "Allowed port : 1024 / 65535" << std::endl;
        return (0);
    }
    return(1);
}

int main(int argc, char const *argv[])
{
    if (argc == 3)
    {
        if (checkport(argv[1]))
        {
            Server server(argv[1], argv[2]);

            server.runningServer();
            return 0;
        }
    }
    else
    {
        std::cout << "Usage: ./ft_irc <port> <password>" << std::endl;
        return 1;
    }

}
