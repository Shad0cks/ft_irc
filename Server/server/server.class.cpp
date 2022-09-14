#include "../includes.hpp"
#include <errno.h>

Server::Server()
{
    int opt = 1;
    int flags;
    this->isRunning = true;
    if((this->socketFD = socket(AF_INET, SOCK_STREAM, 0)) == -1)
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
        
             
        std::cout << "New connection , socket fd is "<< tmpFD << ", ip is : " << inet_ntoa(client_addr.sin_addr) << " , port : " << ntohs(this->serverAddr.sin_port) << std::endl; 
		//send new connection greeting message 
		if( send(tmpFD, "Welcome !", 10, 0) != 10)  
		{  
			perror("send");  
		}  
				
		puts("Welcome message sent successfully");  
				
		//add new socket to array of sockets 
		Client * newClient = new Client;
		newClient->socketFD = tmpFD;
		newClient->clientAddr = client_addr;
		this->connectedClient.insert(std::make_pair(tmpFD, newClient));
    }
}

void Server::disconnectClient(int fd)
{  
	std::cout << "Host disconnected , socket fd is "<< fd << ", ip is : " << inet_ntoa(this->connectedClient[fd]->clientAddr.sin_addr) << " , port : " << ntohs(this->serverAddr.sin_port)<< std::endl;
    this->connectedClient.erase(fd);  
	close(fd);
}

void Server::messageRecieve(void)
{
    int new_socket , activity, i , valread , sd;  
    int max_sd;  
	char buffer[1024];
    struct sockaddr_in address;  
	int addrlen = sizeof(address);
         
    //set of socket descriptors 
    fd_set readfds; 

	while (this->isRunning) 
	{

		//clear the socket set 
        FD_ZERO(&readfds);  
     
        //add master socket to set 
        FD_SET(this->socketFD, &readfds);  
        max_sd = this->socketFD;  
             
        //add child sockets to set 
        for (std::map<int, Client *>::iterator it = this->connectedClient.begin(); it != this->connectedClient.end(); it++)  
        {  
            //socket descriptor 
            sd = it->first;  
            //if valid socket descriptor then add to read list 
            if(sd > 0)  
                FD_SET( sd , &readfds);  
                 
            //highest file descriptor number, need it for the select function 
            if(sd > max_sd)  
                max_sd = sd;  
        }  
		
        //wait for an activity on one of the sockets , timeout is NULL , 
        //so wait indefinitely 
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);  
       
        if ((activity < 0) && (errno!=EINTR))  
        {  
            printf("select error");  
        }  
             
        //If something happened on the master socket , 
        //then its an incoming connection 
        if (FD_ISSET(this->socketFD, &readfds))  
        {  
            this->catchClient();
        }  
		
        //else its some IO operation on some other socket
        for (std::map<int, Client *>::iterator it = this->connectedClient.begin(); it != this->connectedClient.end(); it++)  
        {  
            sd = it->first;  
                 
            if (FD_ISSET( sd , &readfds))  
            {  
                //Check if it was for closing , and also read the 
                //incoming message 
                if ((valread = read( sd , buffer, 1024)) > 0)  
                {  
					if (buffer == std::string("quit"))
					{
						this->disconnectClient(it->first);
						break;
					}
					else
						std::cout << "Client " << it->first << " : " << buffer << std::endl;
                    //Close the socket and mark as 0 in list for reuse 
                }  
                     
                //Echo back the message that came in 
                else 
                {  
                    //set the string terminating NULL byte on the end 
                    //of the data read 
                    buffer[valread] = '\0';  
                    send(sd , buffer , strlen(buffer) , 0 );  
                }  
				valread = 0;
				memset(buffer, 0, 1024);
            }  
        }  
	}
}



