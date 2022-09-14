#ifndef SERVER_CLASS_HPP
#define SERVER_CLASS_HPP

#include "../includes.hpp"
class Client;
class Server
{
    private:
     
        struct sockaddr_in          serverAddr;
        int                         socketFD;
       	std::map<int, Client *>       connectedClient;
        bool                        isRunning;
        std::vector<struct pollfd>  ufds;
        std::string                 password;
        int                         port;

        void                    ExitFailure(std::string message);
        void                    ExitFailure(std::string message, int closedSocket);
    
    public:

        Server(std::string port, std::string password);
        ~Server(void);
        
        void catchClient(void); 
		void disconnectClient(int fd); 
        void messageRecieve(void);
        void checkmessage(std::string message, Client *User);
        
};

#endif