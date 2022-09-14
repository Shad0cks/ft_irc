#include "../includes.hpp"
#include <errno.h>

Server::Server(std::string port, std::string pasword)
{
    int opt = 1;
    int flags;
    this->isRunning = true;
    this->port = std::stoi(port);
    this->password = pasword;
    if((this->socketFD = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        this->ExitFailure("could not create TCP listening socket");
    
    if(setsockopt(this->socketFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
        this->ExitFailure("setsockopt", 1);

    if((flags = fcntl(this->socketFD, F_GETFL)) == -1)
        this->ExitFailure("could not get flags on TCP listening socket", 1);

    if(fcntl(this->socketFD, F_SETFL, flags | O_NONBLOCK))
        this->ExitFailure("could not set TCP listening socket to be non-blocking", 1);

    this->serverAddr.sin_port = htons(PORT);
    this->serverAddr.sin_family = AF_INET;
    this->serverAddr.sin_addr.s_addr = INADDR_ANY;
        
    if (bind(this->socketFD, (struct sockaddr *)&this->serverAddr, sizeof(this->serverAddr)) < 0)
        this->ExitFailure("bind fail", 1);

    if (listen(this->socketFD, 3) < 0)
        this->ExitFailure("listen fail", 1);
    
    struct pollfd tmpPoll;

    tmpPoll.fd = this->socketFD;    
    tmpPoll.events = POLLIN; 
    tmpPoll.revents = 0;
    this->ufds.push_back(tmpPoll);
}

Server::~Server(void)
{
    this->connectedClient.clear();
    this->ufds.clear();
    close(this->socketFD);
}

void Server::ExitFailure(std::string message)
{
    perror(message.c_str());
    exit(EXIT_FAILURE);
}

void Server::ExitFailure(std::string message, int closedSocket)
{
    perror(message.c_str());
    close(this->socketFD);
    exit(EXIT_FAILURE);
}

void Server::catchClient()
{
    int tmpFD;
    
    struct sockaddr_in client_addr;
    socklen_t sizeC = sizeof(client_addr);
    if ((tmpFD = accept(this->socketFD, (struct sockaddr *)&this->serverAddr, &Client::emptySizeSocks)) < 0)
    {    
        if (errno != EWOULDBLOCK)
            this->ExitFailure("error when accepting connection");
    }
    else
    {
        Client client;
        struct pollfd tmpPoll;
        client.socketFD = tmpFD;
        tmpPoll.fd = tmpFD;    
        tmpPoll.events = POLLIN; 
        tmpPoll.revents = 0;
        std::cout << "Server : Client accepted : " << tmpPoll.fd  << std::endl;
        this->connectedClient.push_back(client);
        this->ufds.push_back(tmpPoll);
        // char buffer[1024];
        // int valread;
        // while ((valread = recv(tmpFD, buffer, 1024, 0)) <= 0)
        //     ;
        // buffer[valread] = '\0';
        // std::cout << buffer << std::endl;
    }
}

void Server::messageRecieve(void)
{
    int i, valread = 0;
    char buffer[1024];
    
    while (this->isRunning)
    {
        
        /* errors or timeout? */
        if (poll(this->ufds.begin().base(), this->ufds.size(), 2000) < 0)    
            this->ExitFailure("Error polling fd");

        for (i = 0; i < this->connectedClient.size() + 1; i++)
        {
            /* were there any events for this socket? */
            if (this->ufds[i].revents == 0)
                continue;
            
            if ((this->ufds[i].revents & POLLHUP) == POLLHUP) 
            {
				std::cout << "client disconnected\n";
				break;
			}

            /* is it our listening socket? */
            if ((this->ufds[i].revents & POLLIN) == POLLIN) 
            {
                std::cout << "POLLIN CALL\n";

				if (this->ufds[i].fd == this->socketFD) 
                {
				    this->catchClient();
					break;
				}

				std::cout << "allo ? \n";
                // std::cout << "New message : ";
                // valread = recv(this->ufds[i].fd, buffer, 1024, 0);
                // buffer[valread] = '\0';
                // std::cout << buffer << std::endl;
                // std::cout << "\n";
			}
            
        }
    }
}



