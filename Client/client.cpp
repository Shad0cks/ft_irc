#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <fcntl.h>
#include <poll.h>
#define PORT 30000

int main(int argc, char const *argv[])
{
    int                 socketClient, valread;
    struct sockaddr_in  addrClient;
    int ssize = sizeof(addrClient);

    if ((socketClient = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        return (1);
    }
    addrClient.sin_addr.s_addr = INADDR_ANY;
    addrClient.sin_port = htons(PORT);
    addrClient.sin_family = AF_INET;

    if (connect(socketClient, (sockaddr * ) &addrClient, sizeof(addrClient)) < 0)
    {
        perror("connect");
        return (1);
    }

    std::cout << "Client : Connected to server \n";

    char                buffer[1024];
    int                 i;
    std::string         clientAswer;  
	if ((i = read(socketClient, buffer, 1024)) > 0)
	{
		buffer[i] = '\0';
		std::cout << "Server said : " << buffer << std::endl;
	}
	while (clientAswer != "quit")
    {
        std::cout << "Message : ";
		std::getline(std::cin, clientAswer);
        if (send(socketClient, clientAswer.c_str(), clientAswer.size(), 0) < 0)
        {
            perror("Send ERROR");
            exit(EXIT_FAILURE);
        }
    }

    close(socketClient);   
    std::cout << "Client : Close \n"; 
    return 0;
}
