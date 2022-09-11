#ifndef SERVER_CLASS_HPP
#define SERVER_CLASS_HPP

#include "../includes.hpp"
class Client;
class Server
{
    private:
     
        struct sockaddr_in          serverAddr;
        int                         socketFD;
        std::vector<Client>         connectedClient;
        bool                        isRunning;
        std::vector<struct pollfd>  ufds;

        void                    ExitFailure(std::string message);
        void                    ExitFailure(std::string message, int closedSocket);
    
    public:

        Server(void);
        ~Server(void);
        
        void catchClient(void);  
        void messageRecieve(void);  
        
};

#endif